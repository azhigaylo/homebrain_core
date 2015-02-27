//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "USODefinition.h"
#include "CxLogDev_ExtMod.h"
#include "CxInterfaceManager.h"
//------------------------------------------------------------------------------

#define retry_comm_count   3

//------------------------------------------------------------------------------

CxLogDev_ExtMod::CxLogDev_ExtMod( const char *logDevName, const char *usedInterface, TContExtMod_USO modSettings ):
    CxLogDevice    ( logDevName )
   ,CxDataProvider ( )
   ,CxSysTimer     ( "logDev_EM_Timer", 5000000000, false)  // time in nanosecond
   ,dev_settings   ( modSettings )
   ,commError      ( 0 )
   ,pModBusMaster  ( 0 )
{
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pIxInterface pInterface = pInterfaceManager->get_interface( usedInterface );
   pModBusMaster = dynamic_cast<CxModBusMaster*>(pInterface);

   printDebug("CxLogDev_ExtMod/%s: pModBusMaster=%i", __FUNCTION__, pModBusMaster);

   setExtModStatus( USO_Status_NoReply );
}

void CxLogDev_ExtMod::Process()
{
   if (0 != pModBusMaster)
   {
      // if error count less than retry_comm_count - do request
      if (commError < retry_comm_count)
      {
            if (true == ReadRegisters())
            {
               WriteRegisters();

               // set status for that USO
               setExtModStatus( USO_Status_OK );

               commError = 0;
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
}

void CxLogDev_ExtMod::sigHandler()
{
   commError = 0;
}

//------------------------------------------------------------------------------
bool CxLogDev_ExtMod::WriteRegisters()
{
   bool result = false;

   return result;
}

bool CxLogDev_ExtMod::ReadRegisters()
{
   bool result = false;
   uint16_t mbResponce[dev_settings.recNumb];

   if (0 != dev_settings.pLinkedReg)
   {
      // copy channel data
      TLinkedReg *pCurLinkedReg = dev_settings.pLinkedReg;

      for(uint8_t i=0; i<dev_settings.recNumb; i++,pCurLinkedReg++)
      {
         if (pCurLinkedReg->opType <= LRegToDpoint)
         {
            // read register
            switch (pCurLinkedReg->opType)
            {
               case WordToApoint :
               {
                  uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pCurLinkedReg->strtReg, 1, mbResponce );
                  uint16_t responce = ConvertMBint( mbResponce[0]);
                  
                  float result = static_cast<float>(responce);
                  setAPoint( pCurLinkedReg->NPoint, result );
                  setAStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE );
                  break;
               }
               case LongToApoint:
               {   
                  uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pCurLinkedReg->strtReg, 2, mbResponce );
                  mbResponce[0] = ConvertMBint( mbResponce[0]); 
                  mbResponce[1] = ConvertMBint( mbResponce[1]); 
                  
                  float result = (float)*((long*)mbResponce);
                  setAPoint( pCurLinkedReg->NPoint, result );
                  setAStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE );
                  break;
               }
               case FloatToApoint:
               {
                  uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pCurLinkedReg->strtReg, 2, mbResponce );
                  mbResponce[0] = ConvertMBint( mbResponce[0]); 
                  mbResponce[1] = ConvertMBint( mbResponce[1]); 
                  
                  float result = *((float*)mbResponce);
                  setAPoint( pCurLinkedReg->NPoint, result );
                  setAStatus( pCurLinkedReg->NPoint, STATUS_RELIABLE );
                  break;
               }
               case HRegToDpoint:
               {
                  uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pCurLinkedReg->strtReg, 1, mbResponce );
                  uint16_t responce = ConvertMBint( mbResponce[0]); 
                  
                  setDPoint(pCurLinkedReg->NPoint, HIGH(responce));
                  setDStatus(pCurLinkedReg->NPoint, STATUS_RELIABLE);
                  break;
               }
               case LRegToDpoint:
               {
                  uint16_t reg_num = pModBusMaster->GetRegister( dev_settings.address, pCurLinkedReg->strtReg, 1, mbResponce );
                  uint16_t responce = ConvertMBint( mbResponce[0]); 
                  
                  setDPoint(pCurLinkedReg->NPoint, LOW(responce));
                  setDStatus(pCurLinkedReg->NPoint, STATUS_RELIABLE);
                  break;
               }
               default : break;
            }
         }
      }

      result = true;
   }

   return result;
}

//------------------------------------------------------------------------------      

void CxLogDev_ExtMod::setExtModStatus( uint16_t status )
{
   setDStatus(dev_settings.usoPoint, STATUS_RELIABLE);
   setDPoint(dev_settings.usoPoint, status);
   
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
                  setAStatus( pCurLinkedReg->NPoint, STATUS_UNKNOWN );
                  break;
               }
               case HRegToDpoint :
               case LRegToDpoint :
               case DpointToReg  :
               {
                  setDPoint(pCurLinkedReg->NPoint, STATUS_UNKNOWN);
                  break;
               }
               default : break;
            }
         }
      }
   }
}
