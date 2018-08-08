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
#include "uso/CxLogDev_ExtMod.h"
//------------------------------------------------------------------------------

#define retry_comm_count   3

//------------------------------------------------------------------------------

CxLogDev_ExtMod::CxLogDev_ExtMod( const char *logDevName, const char *usedInterface, TContExtMod_USO modSettings ):
    CxLogDevice    ( logDevName, usedInterface )
   ,CxSysTimer     ( "logDev_EM_Timer", 5000000000, false)  // time in nanosecond
   ,dev_settings   ( modSettings )
   ,commError      ( 0 )
   ,recoveryFlag   (true)
   ,dataProvider   ( CxDataProvider::getInstance() )
   ,pModBusMaster  ( 0 )
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_ExtMod/%s: pModBusMaster=0x%lx", __FUNCTION__, pModBusMaster);
}

CxLogDev_ExtMod::~CxLogDev_ExtMod()
{
   if ( 0 != dev_settings.pLinkedReg)
   {
      // delete table which was created in CxUsoCfgLoader::OpenExtModuleConfig( ... )
      delete [] dev_settings.pLinkedReg;
   }
}

bool CxLogDev_ExtMod::Process()
{
   bool result = false;

   if (0 != pModBusMaster)
   {
      // if error count less than retry_comm_count - do request
      if (commError < retry_comm_count)
      {
          printDebug("CxLogDev_ExtMod/%s: logdev=%s", __FUNCTION__, getDeviceName());
          if (true == ReadWriteRegisters())
          {
             // set status for that USO
             setExtModStatus( USO_Status_OK );

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
            // after mbus error we have to recover outputs
            recoveryFlag = true;
            // set STATUS_UNKNOWN for all channels
            setExtModStatus(USO_Status_NoReply);
            // set delay timer to exclude this module from polling for 5 second
            StartTimer();
            // we inc it to protect cyclic call setUsoStatus and StartTimer
            commError++;
          }
      }
   }

   return result;
}

void CxLogDev_ExtMod::sigHandler()
{
   commError = 0;
}

//------------------------------------------------------------------------------

bool CxLogDev_ExtMod::ReadWriteRegisters()
{
   bool result = false;

   if (0 != dev_settings.pLinkedReg)
   {
      // copy channel data
      TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;

      for(uint8_t i=0; i<dev_settings.recNumb; i++,pCurLinkedReg++)
      {
         // read register
         switch (pCurLinkedReg->opType)
         {
            case WordToApoint :
            {
               result = convertWordToApoint( pCurLinkedReg );
               break;
            }
            case LongToApoint:
            {
               result = convertLongToApoint( pCurLinkedReg );
               break;
            }
            case FloatToApoint:
            {
               result = convertFloatToApoint( pCurLinkedReg );
               break;
            }
            case HRegToDpoint:
            {
               result = convertHRegToDpoint( pCurLinkedReg );
               break;
            }
            case LRegToDpoint:
            {
               result = convertLRegToDpoint( pCurLinkedReg );
               break;
            }
            case ApointToWord:
            {
               if (true == recoveryFlag) result = convertWordToApoint( pCurLinkedReg );
               else result = convertApointToWord( pCurLinkedReg );
               break;
            }
            case ApointToLong:
            {
               if (true == recoveryFlag) result = convertLongToApoint( pCurLinkedReg );
               else result = convertApointToLong( pCurLinkedReg );
               break;
            }
            case ApointToFloat:
            {
               if (true == recoveryFlag) result = convertFloatToApoint( pCurLinkedReg );
               else result = convertApointToFloat( pCurLinkedReg );
               break;
            }
            case DpointToReg:
            {
               if (true == recoveryFlag) result = convertLRegToDpoint( pCurLinkedReg );
               else result = convertDpointToReg( pCurLinkedReg );
               break;
            }
            default : break;
         }

         if (true != result) break;
      }

      if (true == recoveryFlag) recoveryFlag = false;
   }
   else
   {
      printWarning("CxLogDev_ExtMod/%s: pLinkedReg=0 !!!", __FUNCTION__ );
   }

   return result;
}

//------------------------------------------------------------------------------
bool CxLogDev_ExtMod::convertWordToApoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      double value = static_cast<double>(responce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pLinkedReg->NPoint, value);
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertLongToApoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];

   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      double value = (double)*((long*)mbResponce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus(pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pLinkedReg->NPoint, value);
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertFloatToApoint(  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      double value = *((float*)mbResponce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pLinkedReg->NPoint, value);
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertHRegToDpoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      dataProvider.setDPoint( pLinkedReg->NPoint, HIGH(responce));
      dataProvider.setDStatus( pLinkedReg->NPoint, STATUS_RELIABLE);

      printDebug("CxLogDev_ExtMod/%s: DPOINT[%i] = %d", __FUNCTION__, pLinkedReg->NPoint, HIGH(responce));
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertLRegToDpoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      dataProvider.setDPoint( pLinkedReg->NPoint, LOW(responce));
      dataProvider.setDStatus( pLinkedReg->NPoint, STATUS_RELIABLE);

      printDebug("CxLogDev_ExtMod/%s: DPOINT[%i] = %d", __FUNCTION__, pLinkedReg->NPoint, LOW(responce));
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertApointToWord ( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getAStatus(pLinkedReg->NPoint))
   {
      TAPOINT & a_point = dataProvider.getAPoint( pLinkedReg->NPoint );

      uint16_t mbrequest = ConvertMBint((uint16_t)a_point.value);

      if (true == pModBusMaster->SetRegister(dev_settings.address, pLinkedReg->strtReg, mbrequest))
      {
         dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_PROCESSED );
      }
      else
      {
         result = true;
      }
   }

   return result;
}

bool CxLogDev_ExtMod::convertApointToLong ( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getAStatus(pLinkedReg->NPoint))
   {
      uint16_t mbrequest[5];
      TAPOINT & a_point = dataProvider.getAPoint( pLinkedReg->NPoint );

      memcpy(reinterpret_cast<char*>(mbrequest), reinterpret_cast<char*>(&a_point.value), sizeof(a_point.value));
      mbrequest[0] = ConvertMBint(mbrequest[0]);
      mbrequest[1] = ConvertMBint(mbrequest[1]);

      if (true == pModBusMaster->SetRegisterBlock( dev_settings.address, pLinkedReg->strtReg, 2, mbrequest ) )
      {
         dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_PROCESSED );
      }
      else
      {
         result = true;
      }
   }

   return result;
}

bool CxLogDev_ExtMod::convertApointToFloat( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getAStatus(pLinkedReg->NPoint))
   {
      uint16_t mbrequest[5];
      TAPOINT & a_point = dataProvider.getAPoint( pLinkedReg->NPoint );

      *((float*)mbrequest) = a_point.value;
      mbrequest[0] = ConvertMBint(mbrequest[0]);
      mbrequest[1] = ConvertMBint(mbrequest[1]);

      if (true == pModBusMaster->SetRegisterBlock( dev_settings.address, pLinkedReg->strtReg, 2, mbrequest ) )
      {
         dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_PROCESSED );
      }
      else
      {
         result = true;
      }
   }

   return result;
}

bool CxLogDev_ExtMod::convertDpointToReg( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getDStatus(pLinkedReg->NPoint))
   {
      TDPOINT& d_point = dataProvider.getDPoint( pLinkedReg->NPoint );

      uint16_t mbrequest = ConvertMBint((uint16_t)d_point.value);

      if (true == pModBusMaster->SetRegister(dev_settings.address, pLinkedReg->strtReg, mbrequest))
      {
         dataProvider.setDStatus( pLinkedReg->NPoint, STATUS_PROCESSED );
      }
      else
      {
         result = true;
      }
   }

   return result;
}

void CxLogDev_ExtMod::setExtModStatus( uint16_t status )
{
   dataProvider.setDPoint(dev_settings.usoPoint, status);
   dataProvider.setDStatus(dev_settings.usoPoint, STATUS_RELIABLE);

   // set error status for USO
   if ((status == USO_Status_NoReply) && (0 != dev_settings.pLinkedReg))
   {
      TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;

      // set error status for channel
      for (uint8_t i=0; i<dev_settings.recNumb; i++, pCurLinkedReg++)
      {
         // set status
         if (pCurLinkedReg->NPoint)
         {
            switch (pCurLinkedReg->opType)
            {
               case WordToApoint  :
               case LongToApoint  :
               case FloatToApoint :
               case ApointToWord  :
               case ApointToLong  :
               case ApointToFloat :
               {
                  dataProvider.setAStatus( pCurLinkedReg->NPoint, STATUS_UNKNOWN );
                  break;
               }
               case HRegToDpoint :
               case LRegToDpoint :
               case DpointToReg  :
               {
                  dataProvider.setDStatus(pCurLinkedReg->NPoint, STATUS_UNKNOWN);
                  break;
               }
               default : break;
            }
         }
      }
   }
}
