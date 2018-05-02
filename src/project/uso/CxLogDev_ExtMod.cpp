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
   ,dataProvider   ( CxDataProvider::getInstance() )
   ,pModBusMaster  ( 0 )
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_ExtMod/%s: pModBusMaster=%i", __FUNCTION__, pModBusMaster);
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
               result = getWordToApoint ( pCurLinkedReg );
               break;
            }
            case LongToApoint:
            {
               result = getLongToApoint ( pCurLinkedReg );
               break;
            }
            case FloatToApoint:
            {
               result = getFloatToApoint( pCurLinkedReg );
               break;
            }
            case HRegToDpoint:
            {
               result = getHRegToDpoint ( pCurLinkedReg );
               break;
            }
            case LRegToDpoint:
            {
               result = getLRegToDpoint ( pCurLinkedReg );
               break;
            }
            case ApointToWord:
            {
               result =  setApointToWord( pCurLinkedReg );
               break;
            }
            case ApointToLong:
            {
               result =  setApointToLong( pCurLinkedReg );
               break;
            }
            case ApointToFloat:
            {
               result =  setApointToFloat( pCurLinkedReg );
               break;
            }
            case DpointToReg:
            {
               result =  setDpointToReg( pCurLinkedReg );
               break;
            }
            default : break;
         }

         if (true != result) break;
      }
   }
   else
   {
      printWarning("CxLogDev_ExtMod/%s: pLinkedReg=0 !!!", __FUNCTION__ );
   }

   return result;
}

//------------------------------------------------------------------------------
bool CxLogDev_ExtMod::getWordToApoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      float value = static_cast<float>(responce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: WordToApoint = %i", __FUNCTION__, responce );
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::getLongToApoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];

   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      float value = (float)*((long*)mbResponce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus(pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: LongToApoint = %lu", __FUNCTION__, (long)value );
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::getFloatToApoint(  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      float value = *((float*)mbResponce);
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: FloatToApoint = %.4f", __FUNCTION__, value );
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::getHRegToDpoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      dataProvider.setDPoint( pLinkedReg->NPoint, HIGH(responce));
      dataProvider.setDStatus( pLinkedReg->NPoint, STATUS_RELIABLE);

      printDebug("CxLogDev_ExtMod/%s: WordToApoint = %i", __FUNCTION__, responce );
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::getLRegToDpoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
   uint16_t mbResponce[5];
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 1, mbResponce );

   if (reg_num == 1)
   {
      uint16_t responce = ConvertMBint( mbResponce[0] );

      dataProvider.setDPoint( pLinkedReg->NPoint, LOW(responce));
      dataProvider.setDStatus( pLinkedReg->NPoint, STATUS_RELIABLE);

      printDebug("CxLogDev_ExtMod/%s: WordToApoint = %i", __FUNCTION__, responce );
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::setApointToWord ( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getAStatus(pLinkedReg->NPoint))
   {
      uint16_t mbrequest[5];
      TAPOINT & a_point = dataProvider.getAPoint( pLinkedReg->NPoint );

      mbrequest[0] = ConvertMBint((uint16_t)a_point.value);

      if (true == pModBusMaster->SetRegisterBlock( dev_settings.address, pLinkedReg->strtReg, 1, mbrequest ) )
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

bool CxLogDev_ExtMod::setApointToLong ( const TLinkedReg* pLinkedReg )
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

bool CxLogDev_ExtMod::setApointToFloat( const TLinkedReg* pLinkedReg )
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

bool CxLogDev_ExtMod::setDpointToReg( const TLinkedReg* pLinkedReg )
{
   bool result = true;

   if (STATUS_SETNEW == dataProvider.getDStatus(pLinkedReg->NPoint))
   {
      uint16_t mbrequest[5];
      TDPOINT & d_point = dataProvider.getDPoint( pLinkedReg->NPoint );

      mbrequest[0] = ConvertMBint((uint16_t)d_point.value);

      if (true == pModBusMaster->SetRegisterBlock( dev_settings.address, pLinkedReg->strtReg, 1, mbrequest ) )
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
