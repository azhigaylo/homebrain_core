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
   ,counter_item   ( 2 )
   ,sizeResponce   ( 0 )
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

uint16_t CxModBusMaster::GetRegister( uint8_t address, uint16_t reg_start, uint16_t reg_count, uint16_t *pResponce )
{
   uint16_t reg_num = 0;

   pthread_barrier_init(&our_barrier,NULL,2);      
         
   commbuf.msgSize   = sizeof(TMREQ);
   commbuf.msgNumber = counter_item++;

   // set MB request
   mbReadRequest.address       = address; 
   mbReadRequest.command       = CMD_MB_RREG; 
   mbReadRequest.start_reg_hi  = HIGH(reg_start); 
   mbReadRequest.start_reg_low = LOW(reg_start); 
   mbReadRequest.numb_reg_hi   = HIGH(reg_count); 
   mbReadRequest.numb_reg_low  = LOW(reg_count); 
   mbReadRequest.CRC           = CRC16_T(reinterpret_cast<char*>(&mbReadRequest), sizeof(mbReadRequest)-sizeof(mbReadRequest.CRC));
   
   //printDebug("CxModBusMaster/%s: MB send %d %d %d %d %d %d %d", __FUNCTION__, mbReadRequest.address, mbReadRequest.command, mbReadRequest.start_reg_hi,
   //                                                mbReadRequest.start_reg_low, mbReadRequest.numb_reg_hi, mbReadRequest.numb_reg_low, mbReadRequest.numb_b);

   //printDebug("CxModBusMaster/%s: MB CRC=%i / size=%i", __FUNCTION__, mbReadRequest.CRC, sizeof(mbReadRequest)-sizeof(mbReadRequest.CRC) );

   memcpy_m( commbuf.buffer, &mbReadRequest, sizeof mbReadRequest, sizeof commbuf.buffer );

   sendMsg( CM_OUT_DATA, &commbuf );
   
   pthread_barrier_wait(&our_barrier);

   if ( 0 != sizeResponce)
   {
      if (0 != pResponce)
      {
         memcpy_m( pResponce, mbResponce.OutputBuf, sizeResponce, reg_count * sizeof(uint16_t) );
      } 
      reg_num = mbResponce.Header.counter / sizeof(uint16_t);
   }
      
   return reg_num; 
}

bool CxModBusMaster::SetRegister( uint8_t address, uint16_t reg_numb, uint16_t reg_value )
{
   bool result = false;

   pthread_barrier_init(&our_barrier,NULL,2);      

   commbuf.msgSize   = sizeof(TMWRREG);
   commbuf.msgNumber = counter_item++;

   // set MB request
   mbWriteRequest.address       = address; 
   mbWriteRequest.command       = CMD_MB_WREG; 
   mbWriteRequest.start_reg_hi  = HIGH(reg_numb); 
   mbWriteRequest.start_reg_low = LOW(reg_numb); 
   mbWriteRequest.REG           = reg_value; 
   mbWriteRequest.CRC           = CRC16_T(reinterpret_cast<char*>(&mbWriteRequest), sizeof(mbWriteRequest)-sizeof(mbWriteRequest.CRC));
   
   //printDebug("CxModBusMaster/%s: MB send %d %d %d %d %d %d %d", __FUNCTION__, mbReadRequest.address, mbReadRequest.command, mbReadRequest.start_reg_hi,
   //                                                 mbReadRequest.start_reg_low, mbReadRequest.numb_reg_hi, mbReadRequest.numb_reg_low, mbReadRequest.numb_b);

   //printDebug("CxModBusMaster/%s: MB CRC=%i / size=%i", __FUNCTION__, mbReadRequest.CRC, sizeof(mbReadRequest)-sizeof(mbReadRequest.CRC) );

   memcpy_m( commbuf.buffer, &mbWriteRequest, sizeof mbWriteRequest, sizeof commbuf.buffer );

   sendMsg( CM_OUT_DATA, &commbuf );
   
   pthread_barrier_wait(&our_barrier);

   if ( 0 != sizeResponce)
   {
      result = true;
   }
   
   return result; 
}

//------------------------------------------------------------------------------

void CxModBusMaster::CommandProcessor( uint16_t ComID, void *data )
{
   TSerialBlock *pSerialBlock = (TSerialBlock *)data;

   sizeResponce = 0;

   // if we got responce on own ID, we should process it, else - skip it
   if ( commbuf.msgNumber == pSerialBlock->msgNumber )
   {
      switch (ComID)
      {
         case CM_INP_DATA :
         {
            //printDebug("CxModBusMaster/%s: MB receive %d %d %d %d %d %d %d %d %d %d %d %d %d", __FUNCTION__, pSerialBlock->buffer[0],  pSerialBlock->buffer[1],  pSerialBlock->buffer[2],  pSerialBlock->buffer[3],
            //                                                                                                 pSerialBlock->buffer[4],  pSerialBlock->buffer[5],  pSerialBlock->buffer[6],  pSerialBlock->buffer[7],
            //                                                                                                 pSerialBlock->buffer[8],  pSerialBlock->buffer[9],  pSerialBlock->buffer[10], pSerialBlock->buffer[11],
            //                                                                                                 pSerialBlock->buffer[12], pSerialBlock->buffer[13]);

            //printDebug("CxModBusMaster/%s: MB CRC=%i / size=%i", __FUNCTION__, CRC16_T(reinterpret_cast<char*>(pSerialBlock->buffer), pSerialBlock->msgSize-2), pSerialBlock->msgSize-2 );

            if (0 == CRC16_T(reinterpret_cast<char*>(pSerialBlock->buffer), pSerialBlock->msgSize))
            {
               // copy in the internal buffer(rx buffer can receive another data)
               memcpy_m( &mbResponce, pSerialBlock->buffer, pSerialBlock->msgSize, sizeof(mbResponce) );

               if ((mbReadRequest.address == mbResponce.Header.address) && (mbReadRequest.command == mbResponce.Header.command))
               {
                  printDebug("CxModBusMaster/%s: MB got addr=%i / comm=%i / package=%i  ", __FUNCTION__, mbResponce.Header.address, mbResponce.Header.command, pSerialBlock->msgNumber);
                  sizeResponce = pSerialBlock->msgSize;
               }
               else
               {
                  printDebug("CxModBusMaster/%s: MB addr or comm mismatch!", __FUNCTION__ );
               }
            }
            else
            {
               printDebug("CxModBusMaster/%s: MB CRC mismatch!", __FUNCTION__ );
            }
            break;
         }
         case CM_TIMEOUT :
         {
            printDebug("CxModBusMaster/%s: MB timeout addr=%i / comm=%i / package=%i  ", __FUNCTION__, mbResponce.Header.address, mbResponce.Header.command, pSerialBlock->msgNumber);
            break;
         }
         default : printWarning("CxModBusMaster/%s: unexpected cmd ", __FUNCTION__);
      }
   }
   else
   {
      printWarning("CxModBusMaster/%s: cmd was skipped !", __FUNCTION__);	  
   }

   pthread_barrier_wait(&our_barrier);
}

