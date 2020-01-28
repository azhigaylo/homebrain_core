/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

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
   ,firstRegRead   ( 0 )
   ,lastRegRead    ( 0 )
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   // find min and max register numbers
   TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;
   for(uint8_t i=0; i<dev_settings.recNumb; i++,pCurLinkedReg++)
   {
      switch (pCurLinkedReg->opType)
      {
         case WordToApoint:
         case HRegToDpoint:
         case LRegToDpoint:
         case DpointToReg:
         case ApointToWord:
         {
            if (pCurLinkedReg->strtReg < firstRegRead) firstRegRead = pCurLinkedReg->strtReg;
            if (pCurLinkedReg->strtReg >= lastRegRead)  lastRegRead = pCurLinkedReg->strtReg;
            break;
         }
         case LongToApoint:
         case FloatToApoint:
         {
            // these both operation needs 2 modbus registers, so we have to add + 1 to last register
            if (pCurLinkedReg->strtReg < firstRegRead) firstRegRead = pCurLinkedReg->strtReg;
            if (pCurLinkedReg->strtReg >= lastRegRead)  lastRegRead = pCurLinkedReg->strtReg + 1;
            break;
         }
         default : {break;}
      }
   }

   printDebug("CxLogDev_ExtMod/%s: pModBusMaster=0x%lx", __FUNCTION__, pModBusMaster);
   printDebug("CxLogDev_ExtMod/%s: lastRegRead=%i / firstRegRead=%i", __FUNCTION__, lastRegRead, firstRegRead);
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
          if (true == ProcessAllRegisters())
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

bool CxLogDev_ExtMod::ProcessAllRegisters()
{
   bool result = false;

   // if device MB table is small we can try goup all registers in one request
   if ((lastRegRead - firstRegRead) > SUPPORTED_REG_NUM)
   {
      // complex
      result = ReadWriteRegisters();
   }
   else
   {
      // small
      result = ReadRegisters();
      if (true == result)
      {
         WriteRegisters();
      }
   }

   return result;
}

// process complex devices
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

// process simple devices
bool CxLogDev_ExtMod::WriteRegisters()
{
   bool result = true;

   if (0 != dev_settings.pLinkedReg)
   {
      TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;

      for(uint16_t i=0; i<dev_settings.recNumb; i++,pCurLinkedReg++)
      {
         // read register
         switch (pCurLinkedReg->opType)
         {
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

bool CxLogDev_ExtMod::ReadRegisters()
{
   bool result = false;

   if (0 != dev_settings.pLinkedReg)
   {
      // +1 should be added because count is from zero
      result = ReadRegisterBlock(firstRegRead, (lastRegRead - firstRegRead) + 1);
   }
   return result;
}

bool CxLogDev_ExtMod::ReadRegisterBlock( uint16_t start_reg, uint16_t reg_count )
{
   bool result = false;

   printDebug("CxLogDev_ExtMod/%s: StartReg=%i / RegCount=%i", __FUNCTION__, start_reg, reg_count);

   uint16_t reg_num = pModBusMaster->GetRegister(dev_settings.address, start_reg, reg_count, mbResponce);

   if (reg_num == reg_count)
   {
      printDebug("CxLogDev_ExtMod/%s: got %i reister from device", __FUNCTION__, reg_count);

      TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;
      for (uint16_t i = 0; i<dev_settings.recNumb; i++,pCurLinkedReg++)
      {
         uint16_t internal_shift = pCurLinkedReg->strtReg - firstRegRead;
         // read register
         switch (pCurLinkedReg->opType)
         {
            case WordToApoint:
            {
               uint16_t responce = ConvertMBint( mbResponce[internal_shift] );
               double value = static_cast<double>(responce);
               dataProvider.setAPoint( pCurLinkedReg->NPoint, value );
               dataProvider.setAStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE );
               printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pCurLinkedReg->NPoint, value);
               break;
            }
            case HRegToDpoint:
            {
               uint16_t responce = ConvertMBint( mbResponce[internal_shift] );
               dataProvider.setDPoint( pCurLinkedReg->NPoint, HIGH(responce));
               dataProvider.setDStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE);
               printDebug("CxLogDev_ExtMod/%s: DPOINT[%i] = %d", __FUNCTION__, pCurLinkedReg->NPoint, HIGH(responce));
               break;
            }
            case LRegToDpoint:
            {
               uint16_t responce = ConvertMBint( mbResponce[internal_shift] );
               dataProvider.setDPoint( pCurLinkedReg->NPoint, LOW(responce));
               dataProvider.setDStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE);
               printDebug("CxLogDev_ExtMod/%s: DPOINT[%i] = %d", __FUNCTION__, pCurLinkedReg->NPoint, LOW(responce));
               break;
            }
            case LongToApoint:
            {
               mbResponce[internal_shift] = ConvertMBint( mbResponce[internal_shift] );
               mbResponce[internal_shift + 1] = ConvertMBint( mbResponce[internal_shift + 1] );
               double value = *(reinterpret_cast<long*>((void*)(mbResponce + internal_shift)));
               dataProvider.setAPoint( pCurLinkedReg->NPoint, value );
               dataProvider.setAStatus(pCurLinkedReg->NPoint, STATUS_RELIABLE );
               printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pCurLinkedReg->NPoint, value);
               break;
            }
            case FloatToApoint:
            {
               mbResponce[internal_shift] = ConvertMBint( mbResponce[internal_shift] );
               mbResponce[internal_shift+1] = ConvertMBint( mbResponce[internal_shift+1] );
               double value = *(reinterpret_cast<float*>((void*)(mbResponce + internal_shift)));
               dataProvider.setAPoint( pCurLinkedReg->NPoint, value );
               dataProvider.setAStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE );
               printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pCurLinkedReg->NPoint, value);
               break;
            }
            default : break;
         }
      }
      result = true;
   }
   return result;
}

//------------------------------------------------------------------------------
bool CxLogDev_ExtMod::convertWordToApoint (  const TLinkedReg* pLinkedReg  )
{
   bool result = false;
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

bool CxLogDev_ExtMod::convertLongToApoint ( const TLinkedReg* pLinkedReg )
{
   bool result = false;
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      double value = *(reinterpret_cast<long*>((void*)mbResponce));
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus(pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pLinkedReg->NPoint, value);
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertFloatToApoint( const TLinkedReg* pLinkedReg )
{
   bool result = false;
   uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pLinkedReg->strtReg, 2, mbResponce );

   if (reg_num == 2)
   {
      mbResponce[0] = ConvertMBint( mbResponce[0] );
      mbResponce[1] = ConvertMBint( mbResponce[1] );

      double value = *(reinterpret_cast<float*>((void*)mbResponce));
      dataProvider.setAPoint( pLinkedReg->NPoint, value );
      dataProvider.setAStatus( pLinkedReg->NPoint, STATUS_RELIABLE );

      printDebug("CxLogDev_ExtMod/%s: APOINT[%i] = %.4lf", __FUNCTION__, pLinkedReg->NPoint, value);
      result = true;
   }

   return result;
}

bool CxLogDev_ExtMod::convertHRegToDpoint ( const TLinkedReg* pLinkedReg )
{
   bool result = false;
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

bool CxLogDev_ExtMod::convertLRegToDpoint ( const TLinkedReg* pLinkedReg )
{
   bool result = false;
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

      *(reinterpret_cast<float*>((void*)mbrequest)) = a_point.value;;
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
