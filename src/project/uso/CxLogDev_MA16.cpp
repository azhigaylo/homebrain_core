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
#include "uso/CxLogDev_MA16.h"

//------------------------------------------------------------------------------

#define retry_comm_count   3

//------------------------------------------------------------------------------

CxLogDev_MA::CxLogDev_MA( const char *logDevName, const char *usedInterface, TAI_USO usoSettings ):
    CxLogDevice    ( logDevName, usedInterface )
   ,CxSysTimer     ( "logDev_MA_Timer", 5000000000, false)  // time in nanosecond
   ,dev_settings   ( usoSettings )
   ,commError      ( 0 )
   ,dataProvider   ( CxDataProvider::getInstance() )
   ,pModBusMaster  ( 0 )
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_MA/%s: pModBusMaster=0x%lx", __FUNCTION__, pModBusMaster);
}

CxLogDev_MA::~CxLogDev_MA()
{
   if ( 0 != dev_settings.channelsPtr)
   {
      // delete table which was created in CxUsoCfgLoader::OpenAnalModuleConfig( ... )
      delete [] dev_settings.channelsPtr;
   }

}

bool CxLogDev_MA::Process()
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

void CxLogDev_MA::sigHandler()
{
   commError = 0;
}

//------------------------------------------------------------------------------
bool CxLogDev_MA::CheckAndSetOutput()
{
   bool result = false;

   if (0 != dev_settings.channelsPtr)
   {
      TAioChannel *pCurCh = dev_settings.channelsPtr;

      // set error status for channel
      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if (0 != pCurCh->ValPointNumb)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_ANALOG_OUT :
               {
                  if (STATUS_SETNEW == dataProvider.getAStatus(pCurCh->ValPointNumb))
                  {
                    TAPOINT & a_point = dataProvider.getAPoint( pCurCh->ValPointNumb );

                    if (true == pModBusMaster->SetRegister( dev_settings.address, i, static_cast<uint16_t>(a_point.value) ))
                    {
                       dataProvider.setAStatus( pCurCh->ValPointNumb, STATUS_PROCESSED );
                       result = true;
                    }
                  }
                  break;
               }
               case CT_DISCRET_OUT :
               {
                  if (STATUS_SETNEW == dataProvider.getDStatus(pCurCh->ValPointNumb))
                  {
                    TDPOINT & d_point = dataProvider.getDPoint( pCurCh->ValPointNumb );

                    if (true == pModBusMaster->SetRegister( dev_settings.address, i, d_point.value ))
                    {
                       dataProvider.setDStatus( pCurCh->ValPointNumb, STATUS_PROCESSED );
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

bool CxLogDev_MA::ReadRegisters()
{
   bool result = false;

   uint16_t mbResponce[dev_settings.chanNumb];

   // if nothing to set we can do read all MB registers
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, 0, dev_settings.chanNumb, mbResponce );

   if ((0 != dev_settings.channelsPtr) && (reg_num == dev_settings.chanNumb))
   {
      // copy channel data
      TAioChannel *pCurCh = dev_settings.channelsPtr;

      for(uint8_t i=0; i<dev_settings.chanNumb; i++,pCurCh++)
      {
         if ((CT_ANALOG_IN == GetChannelType(pCurCh))||(CT_DISCRET_IN == GetChannelType(pCurCh)))
         {
            pCurCh->Code = ConvertMBint( mbResponce[i]);
         }
      }

      // convert code to value
      ConvAiToVal();
      // convert value to parameter
      ConvAiToParam();

      result = true;
   }

   return result;
}

//------------------------------------------------------------------------------

CxLogDev_MA::TChType CxLogDev_MA::GetChannelType( const TAioChannel *pCurCh )
{
   TChType chType = CT_UNUSED;

   switch (pCurCh->Type)
   {
      case ATYPE_AI_20mA :
      case ATYPE_AI_5mA :
      case ATYPE_AI_pm10V:
      case ATYPE_AI_01V:
      case ATYPE_AI_36V:
      case ATYPE_AI_120V:
      case ATYPE_AI_D300V:
      case ATYPE_AI_TCM100:
      case ATYPE_AI_A300V:
      case ATYPE_AI_Gen:
      case ATYPE_AI_Freq:
      case ATYPE_AI_20mA_Z:
      case ATYPE_AI_TCP100:
      case ATYPE_AI_TCP100_A:{ chType = CT_ANALOG_IN ;   break; }

      case ATYPE_DI1:        { chType = CT_DISCRET_IN;   break; }
      case ATYPE_DO1:        { chType = CT_DISCRET_OUT;  break; }

      case ATYPE_AO_Gen:
      case ATYPE_AO_Linear:
      case ATYPE_AO_100pc:
      case ATYPE_AO_10V:
      case ATYPE_AO_20mA:    { chType = CT_ANALOG_OUT;   break; }

      case ATYPE_unuse:
      default:               { chType = CT_UNUSED; break; }
   }

   return chType;
}

void CxLogDev_MA::setUsoStatus( uint16_t status )
{
   setDevStatus( status );

   dataProvider.setDStatus(dev_settings.usoPoint, STATUS_RELIABLE);
   dataProvider.setDPoint(dev_settings.usoPoint, status);

   // set error status for USO
   if ((status == USO_Status_NoReply) && (0 != dev_settings.channelsPtr))
   {
      TAioChannel *pCurCh = dev_settings.channelsPtr;

      // set error status for channel
      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         // set status
         if (0 != pCurCh->ValPointNumb)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_ANALOG_IN  :
               case CT_ANALOG_OUT :
               {
                  dataProvider.setAStatus( pCurCh->ValPointNumb, STATUS_UNKNOWN );
                  if (0 != pCurCh->MidPointNumb)
                  {
                     dataProvider.setAStatus( pCurCh->MidPointNumb, STATUS_UNKNOWN );
                  }
                  break;
               }
               case CT_DISCRET_IN  :
               case CT_DISCRET_OUT :
               {
                  dataProvider.setDPoint(pCurCh->ValPointNumb, STATUS_UNKNOWN);
                  break;
               }
               default : break;
            }
         }
      }
   }
}

// procedure conversion analog parameter in value
void CxLogDev_MA::ConvAiToVal()
{
   if (0 !=  dev_settings.channelsPtr)
   {
      TAioChannel *pCurCh = dev_settings.channelsPtr;

      //chanel processing
      for (uint8_t i=0; i<dev_settings.chanNumb; i++)
      {
         switch (pCurCh->Type)
         {
            case ATYPE_AI_20mA     : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_4_20);   break;}
            case ATYPE_AI_20mA_Z   : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_4_20_Z); break;}
            case ATYPE_AI_5mA      : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_0_5);    break;}
            case ATYPE_AI_pm10V    : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_10_10 - 6.6666);   break;}
            case ATYPE_AI_01V      : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_0_01);   break;}
            case ATYPE_AI_36V      : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_0_36);   break;}
            case ATYPE_AI_120V     : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_0_120);  break;}
            case ATYPE_AI_D300V    : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_DC_300); break;}
            case ATYPE_AI_TCM100   : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_TCP100 + 1.854);   break;}
            case ATYPE_AI_TCP100_A : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_TCP100_A + 1.854); break;}
            case ATYPE_AI_A300V    : {pCurCh->MidValue = static_cast<float>(pCurCh->Code * AI_AC_300); break;}
            default                : {break;}
         }

         pCurCh->ChanelStatus = A_PARAM_RELIABLE;      //preset status of parameter

         if (pCurCh->MidValue < pCurCh->MinMid) pCurCh->ChanelStatus = A_PARAM_LESSCODESP;     //analise - less code sp
         if (pCurCh->MidValue > pCurCh->MaxMid) pCurCh->ChanelStatus = A_PARAM_MORECODESP;     //analise - more code sp

         // store value
         if (0 != pCurCh->MidPointNumb)
         {
            if ((CT_DISCRET_OUT != GetChannelType(pCurCh))&&(CT_DISCRET_IN != GetChannelType(pCurCh)))
            {
               if (pCurCh->ChanelStatus == A_PARAM_RELIABLE)
               {
                  dataProvider.setAStatus( pCurCh->MidPointNumb, STATUS_RELIABLE );
               }
               else
               {
                  dataProvider.setAStatus( pCurCh->MidPointNumb, STATUS_ALARM );
               }

               dataProvider.setAPoint( pCurCh->MidPointNumb, pCurCh->MidValue );
            }
         }
         pCurCh++;
      }
   }
}

// rocedure conversion analog parameter in parameter
void CxLogDev_MA::ConvAiToParam()
{
   if (0 !=  dev_settings.channelsPtr)
   {
      TAioChannel *pCurCh = dev_settings.channelsPtr;

      for (uint8_t i=0; i<dev_settings.chanNumb; i++, pCurCh++)
      {
         switch (pCurCh->Type)
         {
            case ATYPE_AI_20mA     : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/(pCurCh->MaxMid - pCurCh->MinMid))*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_20mA_Z   : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/(pCurCh->MaxMid - pCurCh->MinMid))*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_5mA      : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/5)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);    break;}
            case ATYPE_AI_pm10V    : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/20)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal*2); break;}
            case ATYPE_AI_01V      : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/0.1)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_36V      : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/36)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);   break;}
            case ATYPE_AI_120V     : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/120)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_D300V    : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/300)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_A300V    : { pCurCh->PhisValue = static_cast<float>(((pCurCh->MidValue - pCurCh->MinMid)/300)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal);  break;}
            case ATYPE_AI_TCM100   : { pCurCh->PhisValue = static_cast<float>(K - (float)sqrt(K2 + (float)(pCurCh->MidValue/R0-1.0)/B)); break;}
            case ATYPE_AI_TCP100_A : { pCurCh->PhisValue = static_cast<float>(K - (float)sqrt(K2 + (float)(pCurCh->MidValue/R0-1.0)/B)); break;}
            default                : { break; }
         }

         pCurCh->ChanelStatus = A_PARAM_RELIABLE;   // set status of parameter

         // analyse - less code sp
         if (pCurCh->PhisValue < pCurCh->MinVal){pCurCh->ChanelStatus = A_PARAM_LESSVALSP;}
         // analyse - more code sp
         if (pCurCh->PhisValue > pCurCh->MaxVal){pCurCh->ChanelStatus = A_PARAM_MOREVALSP;}

         // store value
         if (0 != pCurCh->ValPointNumb)
         {
            switch (GetChannelType(pCurCh))
            {
               case CT_ANALOG_IN  :
               {
                  if (pCurCh->ChanelStatus == A_PARAM_RELIABLE)
                  {
                     dataProvider.setAStatus( pCurCh->ValPointNumb, STATUS_RELIABLE );
                  }
                  else
                  {
                     dataProvider.setAStatus( pCurCh->ValPointNumb, STATUS_ALARM );
                  }

                  dataProvider.setAPoint( pCurCh->ValPointNumb, pCurCh->PhisValue );
                  break;
               }
               case CT_DISCRET_IN  :
               {
                  if ((pCurCh->Code) & 0x0001)
                  {
                     dataProvider.setDPoint(pCurCh->ValPointNumb, 0x01);
                  }
                  else
                  {
                     dataProvider.setDPoint(pCurCh->ValPointNumb, 0x00);
                  }

                  dataProvider.setDStatus(pCurCh->ValPointNumb, STATUS_RELIABLE);
                  break;
               }
               default : break;
            }
         }

      }
   }
}
