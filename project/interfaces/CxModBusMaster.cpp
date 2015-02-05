//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxModBusMaster.h"
//------------------------------------------------------------------------------

CxModBusMaster::CxModBusMaster( const char *interfaceName, const char *drvName ):
    CxThreadIO     ( interfaceName, drvName )
   ,CxInterface    ( interfaceName )
   ,IxEventProducer( )
   ,backEvent      ( event_pool::EVENT_DUMMY )
   ,busMasterState ( ST_MM_FREE )
{
   memset_m( &commbuf, 0, sizeof commbuf, sizeof commbuf );
}

int32_t CxModBusMaster::open( )
{
   Start();
}

int32_t CxModBusMaster::close( )
{
   printWarning("CxModBusMaster/%s: CxModBusMaster close not implemented", __FUNCTION__);
}

bool CxModBusMaster::SetBackEvent( eEventType event )
{
   bool result = false;
   
   if (busMasterState == ST_MM_FREE)
   {
      backEvent = event;
      busMasterState = ST_IO_WAITFOR_RQ;
      result = true;
   }
   else
   {
      printWarning("CxModBusMaster/%s: already busy !!!", __FUNCTION__);
   }

   return result; 
}

void CxModBusMaster::Unblock()
{
   busMasterState = ST_MM_FREE;
}

uint16_t counter_item = 2;
TSerialBlock serialBlock;
   
bool CxModBusMaster::GetRegister( uint16_t reg_start, uint16_t reg_count )
{
   bool result = true;

   if (busMasterState == ST_IO_WAITFOR_RQ)
   {
      serialBlock.msgSize   = 5;
      serialBlock.msgNumber = counter_item++;

      sendMsg( CM_OUT_DATA, &serialBlock );

      msgIdInProcessing = serialBlock.msgNumber;

      busMasterState = ST_IO_WAITFOR_RESP;
   }
   else
   {
      printWarning("CxModBusMaster/%s: back event must be set before !!!", __FUNCTION__);
   }

   return result; 
}

//------------------------------------------------------------------------------

void CxModBusMaster::CommandProcessor( uint16_t ComID, void *data )
{
   TSerialBlock *pSerialBlock = (TSerialBlock *)data;

   if (busMasterState == ST_IO_WAITFOR_RESP)
   {
      // if we got responce on oun ID, we should process it, else - skip it
      if ( msgIdInProcessing == pSerialBlock->msgNumber)
      {
         // copy in the internal buffer(rx buffer can receive another data)
         commbuf.msgSize = pSerialBlock->msgSize;
         commbuf.msgNumber = pSerialBlock->msgNumber;
         memcpy_m( &commbuf, pSerialBlock->buffer, pSerialBlock->msgSize, sizeof(commbuf) );

         switch (ComID)
         {
            case CM_INP_DATA :
            {
               printDebug("CxModBusMaster/%s: rd size=%i, package=%i ", __FUNCTION__, pSerialBlock->msgSize, pSerialBlock->msgNumber);
               sendEvent( backEvent, &commbuf );
               break;
            }
            case CM_TIMEOUT :
            {
               printDebug("CxModBusMaster/%s: timeout for package=%i ", __FUNCTION__, pSerialBlock->msgSize, pSerialBlock->msgNumber);
               sendEvent( backEvent, &commbuf );
               break;
            }
            default : printWarning("CxModBusMaster/%s: unexpected cmd ", __FUNCTION__);
         }

         busMasterState = ST_IO_BLOCKED;
      }
      else
      {
         printWarning("CxModBusMaster/%s: cmd was skipped !", __FUNCTION__);	  
      }
   }
}

