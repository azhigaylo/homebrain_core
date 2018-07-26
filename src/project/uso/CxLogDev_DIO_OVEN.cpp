//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "interface/CxInterfaceManager.h"

#include "uso/USODefinition.h"
#include "uso/CxLogDev_DIO_OVEN.h"

//------------------------------------------------------------------------------

#define retry_comm_count   3

//------------------------------------------------------------------------------

CxLogDev_DIO_OVEN::CxLogDev_DIO_OVEN( const char *logDevName, const char *usedInterface, TDIO_USO usoSettings ):
    CxLogDevice    ( logDevName, usedInterface )
   ,CxSysTimer     ( "logDev_DIO_Timer", 5000000000, false)  // time in nanosecond
   ,dev_settings   ( usoSettings )
   ,commError      ( 0 )
   ,dataProvider   ( CxDataProvider::getInstance() )
   ,pModBusMaster  ( 0 )
   ,dev_type       (GetDeviceType(usoSettings))
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_DIO_OVEN/%s: device type=%s(%d), interface pModBusMaster=0x%lx", __FUNCTION__,
                                                                                   deviceType2Str(dev_type).c_str(),
                                                                                   dev_type,
                                                                                   pModBusMaster);
}

CxLogDev_DIO_OVEN::~CxLogDev_DIO_OVEN()
{
   if ( 0 != dev_settings.channelsPtr)
   {
      // delete table which was created in CxUsoCfgLoader::OpenDiscretModuleConfig( ... )
      delete [] dev_settings.channelsPtr;
   }

}

bool CxLogDev_DIO_OVEN::Process()
{
   bool result = false;

   if (0 != pModBusMaster)
   {
      // if error count less than retry_comm_count - do request
      if (commError < retry_comm_count)
      {
            printDebug("CxLogDev_DIO_OVEN/%s: logdev=%s", __FUNCTION__, getDeviceName());
            if (true == ProcessDevice())
            {
               // set status for that USO
               setUsoStatus( USO_Status_OK );

               commError = 0;
               result = true;
            }
            else
            {
               commError++;           // increment error counter
            }
      }
      else
      {
         if (commError == retry_comm_count)
         {
            // set STATUS_UNKNOWN for all channels
            setUsoStatus(USO_Status_NoReply);
            // set delay timer to exclude this module from polling for 5 second
            StartTimer();
            // we inc it to protect cyclic call setUsoStatus and StartTimer
            commError++;
         }
      }
   }

   return result;
}

void CxLogDev_DIO_OVEN::sigHandler()
{
   commError = 0;
}

//------------------------------------------------------------------------------

bool CxLogDev_DIO_OVEN::ProcessDevice()
{
   bool result = false;

   switch(dev_type)
   {
      case DT_INPUT:
      {
         result = ProcessDiDevice();
         break;
      }
      case DT_OUTPUT:
      {
         result = ProcessDoDevice();
         break;
      }
      case DT_MIXED:
      {
         if (true == ProcessDiDevice())
         {
            result = ProcessDoDevice();
         }
         break;
      }
      default : break;
   }
   return result;
}

bool CxLogDev_DIO_OVEN::ProcessDiDevice()
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t data_register = 0;

   // if nothing to set we can do read all MB registers
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, discret_input_reg, 1, mbResponce );

   data_register = ConvertMBint(mbResponce[0]);

   printDebug("CxLogDev_DIO_OVEN/%s: logdev=%s, reg 0x33 = %i", __FUNCTION__, getDeviceName(), data_register);

   if ((0 != dev_settings.channelsPtr) && (reg_num == 1))
   {
      // copy channel data
      TDioChannel *pCurCh = dev_settings.channelsPtr;

      for(uint8_t i=0; i<dev_settings.chanNumb; i++,pCurCh++)
      {
         if ((0 != pCurCh->PointNumb) && (CT_DISCRET_IN == GetChannelType(pCurCh)))
         {
            if (data_register & (1<<i))
            {
               dataProvider.setDPoint(pCurCh->PointNumb, 1);
            }
            else
            {
               dataProvider.setDPoint(pCurCh->PointNumb, 0);
            }
            dataProvider.setDStatus(pCurCh->PointNumb, STATUS_RELIABLE);
         }
      }
      result = true;
   }

   return result;
}

bool CxLogDev_DIO_OVEN::ProcessDoDevice()
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t initial_mask = 0;
   uint16_t output_mask  = 0;

   // if nothing to set we can do read all MB registers
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, discret_output_reg, 1, mbResponce );

   // preset output mask
   initial_mask = output_mask = ConvertMBint(mbResponce[0]);

   printDebug("CxLogDev_DIO_OVEN/%s: logdev=%s, reg 0x32 = %d", __FUNCTION__, getDeviceName(), output_mask);

   if ((0 != dev_settings.channelsPtr) && (reg_num == 1))
   {
      TDioChannel *pCurCh = dev_settings.channelsPtr;
      // set error status for channel
      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if ((0 != pCurCh->PointNumb) && (CT_DISCRET_OUT == GetChannelType(pCurCh)))
         {
            if (STATUS_SETNEW == dataProvider.getDStatus(pCurCh->PointNumb))
            {
               TDPOINT& d_point = dataProvider.getDPoint( pCurCh->PointNumb );

               printDebug("CxLogDev_DIO_OVEN/%s: detected new value=%i in channel %i", __FUNCTION__, d_point.value, i);

               dataProvider.setDStatus( pCurCh->PointNumb, STATUS_PROCESSED );
               if (0 != d_point.value )
               {
                   output_mask |= (1 << i);
               }
               else
               {
                   output_mask &= ~(1 << i);
               }
            }
         }
      }

      if (initial_mask != output_mask )
      {
          printDebug("CxLogDev_DIO_OVEN/%s: new output_mask = %i", __FUNCTION__, output_mask);

          output_mask = ConvertMBint(output_mask);
          if (true == pModBusMaster->SetRegisterBlock( dev_settings.address, discret_output_reg, 1, &output_mask ))
          {
             printDebug("CxLogDev_DIO_OVEN/%s: logdev=%s, write reg 0x32 = %i", __FUNCTION__, getDeviceName(), output_mask);
             result = true;
          }
      }
      else
      {
         result = true;
      }
   }

   return result;
}

//------------------------------------------------------------------------------

CxLogDev_DIO_OVEN::TChType CxLogDev_DIO_OVEN::GetChannelType( const TDioChannel *pCurCh )
{
   TChType chType = CT_UNUSED;

   switch (pCurCh->Type)
   {
      case ATYPE_DI1:        { chType = CT_DISCRET_IN;   break; }
      case ATYPE_DO1:        { chType = CT_DISCRET_OUT;  break; }

      case ATYPE_unuse:
      default:               { chType = CT_UNUSED; break; }
   }

   return chType;
}

CxLogDev_DIO_OVEN::TDeviceType CxLogDev_DIO_OVEN::GetDeviceType( TDIO_USO settings )
{
   TDeviceType result = DT_UNKNOWN;
   uint8_t output_ch  = 0;
   uint8_t input_ch   = 0;

   if (0 != settings.channelsPtr)
   {
      TDioChannel *pCurCh = settings.channelsPtr;
      // set error status for channel
      for (uint8_t i=0; i<settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if (0 != pCurCh->PointNumb)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_DISCRET_OUT :
               {
                  output_ch++;
                  break;
               }
               case CT_DISCRET_IN :
               {
                  input_ch++;
                  break;
               }
               default : break;
            }
         }
      }

      if (0 != output_ch)
      {
         if (0 != input_ch)
         {
            result = DT_MIXED;
         }
         else
         {
            result = DT_OUTPUT;
         }
      }
      else
      {
         result = DT_INPUT;
      }
   }

   return result;
}

void CxLogDev_DIO_OVEN::setUsoStatus( uint16_t status )
{
   setDevStatus( status );

   dataProvider.setDStatus(dev_settings.usoPoint, STATUS_RELIABLE);
   dataProvider.setDPoint(dev_settings.usoPoint, status);

   // set error status for USO
   if ((status == USO_Status_NoReply) && (0 != dev_settings.channelsPtr))
   {
	   TDioChannel *pCurCh = dev_settings.channelsPtr;

      // set error status for channel
      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if (0 != pCurCh->PointNumb)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_DISCRET_IN  :
               case CT_DISCRET_OUT :
               {
                  dataProvider.setDStatus(pCurCh->PointNumb, STATUS_UNKNOWN);
                  break;
               }
               default : break;
            }
         }
      }
   }
}

std::string CxLogDev_DIO_OVEN::deviceType2Str(TDeviceType d_type)
{
   std::string str;

   switch (d_type)
   {
      case DT_UNKNOWN : { str = "DT_UNKNOWN" ; break;}
      case DT_INPUT   : { str = "DT_INPUT";    break;}
      case DT_OUTPUT  : { str = "DT_OUTPUT";   break;}
      case DT_MIXED   : { str = "DT_MIXED";    break;}
   }
   return str;
}

