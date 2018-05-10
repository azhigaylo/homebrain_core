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
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_DIO_OVEN/%s: pModBusMaster=0x%lx", __FUNCTION__, pModBusMaster);
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
            printDebug("CxLogDev_ExtMod/%s: logdev=%s", __FUNCTION__, getDeviceName());
            if (true == ReadRegisters())
            {
               CheckAndSetOutput();
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
bool CxLogDev_DIO_OVEN::CheckAndSetOutput()
{
   bool result = false;

   if (0 != dev_settings.channelsPtr)
   {
	   TDioChannel *pCurCh = dev_settings.channelsPtr;

      // set error status for channel
      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if (0 != pCurCh->Number)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_DISCRET_OUT :
               {
                  if (STATUS_SETNEW == dataProvider.getDStatus(pCurCh->Number))
                  {
                    TDPOINT & d_point = dataProvider.getDPoint( pCurCh->Number );

                    if (true == pModBusMaster->SetRegister( dev_settings.address, i, static_cast<uint16_t>(d_point.value) ))
                    {
                       dataProvider.setDStatus( pCurCh->Number, STATUS_PROCESSED );
                       result = true;
                    }
                  }
                  break;
               }
               default : break;
            }
         }
      }
   }

   return result;
}

bool CxLogDev_DIO_OVEN::ReadRegisters()
{
   bool result = false;

   uint16_t mbResponce[dev_settings.chanNumb];

   // if nothing to set we can do read all MB registers
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, 0x52, 1, mbResponce );

   if ((0 != dev_settings.channelsPtr) && (reg_num == 1))
   {
      // copy channel data
      TDioChannel *pCurCh = dev_settings.channelsPtr;

      for(uint8_t i=0; i<dev_settings.chanNumb; i++,pCurCh++)
      {
         if (CT_DISCRET_IN == GetChannelType(pCurCh))
         {
            pCurCh->Code = ConvertMBint( mbResponce[i]);
         }
      }

      // process register data
      // TODO

      result = true;
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
         if (0 != pCurCh->Number)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_DISCRET_IN  :
               case CT_DISCRET_OUT :
               {
                  dataProvider.setDPoint(pCurCh->Number, STATUS_UNKNOWN);
                  break;
               }
               default : break;
            }
         }
      }
   }
}

