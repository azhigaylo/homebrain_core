//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "interfaces/CxModBusMaster.h"
//------------------------------------------------------------------------------

CxModBusMaster::CxModBusMaster( const char *interfaceName, const char *drvName )
   :CxThreadIO     ( interfaceName, drvName )
   ,CxInterface    ( interfaceName )
   ,counter_item   ( 2 )
   ,sizeResponce   ( 0 )
   ,last_command   ( CMD_MB_NULL )
   ,last_address   ( 0 )
   ,cond_mutex     ( PTHREAD_MUTEX_INITIALIZER )
   ,cond_var       ( PTHREAD_COND_INITIALIZER )
   ,cond_var_flag  ( 0 )
{
   memset( &commbuf, 0, sizeof commbuf );
}

CxModBusMaster::~CxModBusMaster()
{
   close( );
}

int32_t CxModBusMaster::open( )
{
   Start();
   return 0;
}

int32_t CxModBusMaster::close( )
{
   Stop();
   return 0;
}

uint16_t CxModBusMaster::GetRegister( uint8_t address, uint16_t reg_start, uint16_t reg_count, uint16_t *pResponce )
{
   uint16_t reg_num = 0;

   commbuf.msgSize   = sizeof(TMREQ);
   commbuf.msgNumber = counter_item++;

   // set MB request
   mbReadRequest.address       = last_address = address;
   mbReadRequest.command       = last_command = CMD_MB_RREG;
   mbReadRequest.start_reg_hi  = HIGH(reg_start);
   mbReadRequest.start_reg_low = LOW(reg_start);
   mbReadRequest.numb_reg_hi   = HIGH(reg_count);
   mbReadRequest.numb_reg_low  = LOW(reg_count);
   mbReadRequest.CRC           = CRC16_T(reinterpret_cast<char*>(&mbReadRequest), sizeof(mbReadRequest)-sizeof(mbReadRequest.CRC));

   printDebug("CxModBusMaster/%s: MB(%s) send(%d byte) %d %d %d %d %d %d + crc(0x%x)",
                                                                       __FUNCTION__,
                                                                       getInterfaceName(),
                                                                       sizeof(mbReadRequest),
                                                                       mbReadRequest.address,
                                                                       mbReadRequest.command,
                                                                       mbReadRequest.start_reg_hi,
                                                                       mbReadRequest.start_reg_low,
                                                                       mbReadRequest.numb_reg_hi,
                                                                       mbReadRequest.numb_reg_low,
                                                                       mbReadRequest.CRC);

   memcpy(commbuf.buffer, &mbReadRequest, sizeof mbReadRequest);

   // send message to serial driver
   if (true == sendMsg( CM_OUT_DATA, &commbuf ))
   {
      sleep_till_resp();

      if ( 0 != sizeResponce)
      {
         if (0 != pResponce)
         {
            memcpy( pResponce, mbResponce.OutputBuf, sizeResponce );
         }
         reg_num = static_cast<uint8_t>(mbResponce.Header.counter / (uint8_t)(sizeof(uint16_t)));
      }
   }
   return reg_num;
}

bool CxModBusMaster::SetRegister( uint8_t address, uint16_t reg_numb, uint16_t reg_value )
{
   bool result = false;

   commbuf.msgSize   = sizeof(TMWRREG);
   commbuf.msgNumber = counter_item++;

   // set MB request
   mbWriteRequest.address       = last_address = address;
   mbWriteRequest.command       = last_command = CMD_MB_WREG;
   mbWriteRequest.start_reg_hi  = HIGH(reg_numb);
   mbWriteRequest.start_reg_low = LOW(reg_numb);
   mbWriteRequest.REG           = reg_value;
   mbWriteRequest.CRC           = CRC16_T(reinterpret_cast<char*>(&mbWriteRequest), sizeof(mbWriteRequest)-sizeof(mbWriteRequest.CRC));

   printDebug("CxModBusMaster/%s: MB(%s) send(%d byte) %d %d %d %d %i + crc(0x%x)",
                                                                                   __FUNCTION__,
                                                                                   getInterfaceName(),
                                                                                   sizeof(mbWriteRequest),
                                                                                   mbWriteRequest.address,
                                                                                   mbWriteRequest.command,
                                                                                   mbWriteRequest.start_reg_hi,
                                                                                   mbWriteRequest.start_reg_low,
                                                                                   mbWriteRequest.REG,
                                                                                   mbWriteRequest.CRC);

   memcpy( commbuf.buffer, &mbWriteRequest, sizeof mbWriteRequest);

   // send message to serial driver
   sendMsg( CM_OUT_DATA, &commbuf );

   sleep_till_resp();

   if ( 0 != sizeResponce)
   {
      result = true;
   }

   return result;
}

bool CxModBusMaster::SetRegisterBlock( uint8_t address, uint16_t reg_start, uint16_t reg_count, const uint16_t *pOutput )
{
   bool result = false;

   // set MB request
   mbWrBlkReg.Header.address       = last_address = address;
   mbWrBlkReg.Header.command       = last_command = CMD_MB_WARRREG;
   mbWrBlkReg.Header.start_reg_hi  = HIGH(reg_start);
   mbWrBlkReg.Header.start_reg_low = LOW(reg_start);
   mbWrBlkReg.Header.numb_reg_hi   = HIGH(reg_count);
   mbWrBlkReg.Header.numb_reg_low  = LOW(reg_count);
   mbWrBlkReg.Header.NumbB         = (uint8_t)(reg_count*sizeof(uint16_t));

   // copy buffer
   memcpy( mbWrBlkReg.OutputBuf, pOutput, mbWrBlkReg.Header.NumbB );
   //setup CRC
   mbWrBlkReg.OutputBuf[reg_count] = CRC16_T(reinterpret_cast<char*>(&mbWrBlkReg), (uint16_t)(sizeof(mbWrBlkReg.Header) + mbWrBlkReg.Header.NumbB));

   // copy to communication buffer
   commbuf.msgSize   = (uint16_t)(sizeof(mbWrBlkReg.Header) + mbWrBlkReg.Header.NumbB + sizeof(uint16_t));   // sizeof(uint16_t) - CRC size
   commbuf.msgNumber = counter_item++;

   memcpy( commbuf.buffer, &mbWrBlkReg, commbuf.msgSize);

   printDebug("CxModBusMaster/%s: MB(%s) send(%d byte) %d %d %d %d %i + crc(0x%x)",
                                                                                   __FUNCTION__,
                                                                                   getInterfaceName(),
                                                                                   sizeof(mbWriteRequest),
                                                                                   mbWrBlkReg.Header.address,
                                                                                   mbWrBlkReg.Header.command,
                                                                                   mbWrBlkReg.Header.start_reg_hi,
                                                                                   mbWrBlkReg.Header.start_reg_low,
                                                                                   mbWrBlkReg.Header.NumbB,
                                                                                   mbWrBlkReg.OutputBuf[reg_count]);

   // send message to serial driver
   sendMsg( CM_OUT_DATA, &commbuf );

   sleep_till_resp();

   if ( 0 != sizeResponce)
   {
      result = true;
   }

   return result;
}

//------------------------------------------------------------------------------

void CxModBusMaster::sleep_till_resp()
{
  pthread_mutex_lock(&cond_mutex);

  cond_var_flag = 0;
  while (cond_var_flag == 0)
  {
     pthread_cond_wait(&cond_var, &cond_mutex);
  }

  pthread_mutex_unlock(&cond_mutex);
}

void CxModBusMaster::waikeup_by_serial()
{
  pthread_mutex_lock(&cond_mutex);

  cond_var_flag++;

  pthread_cond_signal(&cond_var);

  pthread_mutex_unlock(&cond_mutex);
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
            printDebug("CxModBusMaster/%s: MB(%s) receive %d %d %d %d %d %d %d %d %d %d %d %d %d", __FUNCTION__, getInterfaceName(),
                                  pSerialBlock->buffer[0],  pSerialBlock->buffer[1],  pSerialBlock->buffer[2],  pSerialBlock->buffer[3],
                                  pSerialBlock->buffer[4],  pSerialBlock->buffer[5],  pSerialBlock->buffer[6],  pSerialBlock->buffer[7],
                                  pSerialBlock->buffer[8],  pSerialBlock->buffer[9],  pSerialBlock->buffer[10], pSerialBlock->buffer[11],
                                  pSerialBlock->buffer[12], pSerialBlock->buffer[13]);

            printDebug("CxModBusMaster/%s: MB(%s) CRC=%i / size=%i" , __FUNCTION__,
                                                                      getInterfaceName(),
                                                                      CRC16_T(reinterpret_cast<char*>(pSerialBlock->buffer),
                                                                      (uint16_t)(pSerialBlock->msgSize-2)),
                                                                      pSerialBlock->msgSize-2 );

            if (0 == CRC16_T(reinterpret_cast<char*>(pSerialBlock->buffer), pSerialBlock->msgSize))
            {
               // copy in the internal buffer(rx buffer can receive another data)
               memcpy( &mbResponce, pSerialBlock->buffer, pSerialBlock->msgSize );

               if ((last_address == mbResponce.Header.address) && (last_command == mbResponce.Header.command))
               {
                  printDebug("CxModBusMaster/%s: MB(%s) got addr=%i / comm=%i / package=%i  ", __FUNCTION__,
                                                                                               getInterfaceName(),
                                                                                               mbResponce.Header.address,
                                                                                               mbResponce.Header.command,
                                                                                               pSerialBlock->msgNumber);

                  sizeResponce = pSerialBlock->msgSize;
               }
               else
               {
                  printWarning("CxModBusMaster/%s: MB(%s) addr or comm mismatch", __FUNCTION__, getInterfaceName() );
               }
            }
            else
            {
               printWarning("CxModBusMaster/%s: MB(%s) CRC mismatch!", __FUNCTION__, getInterfaceName() );
            }
            break;
         }
         case CM_TIMEOUT :
         {
            printWarning("CxModBusMaster/%s: MB(%s) timeout addr=%i / comm=%i / package=%i  ", __FUNCTION__,
                                                                                               getInterfaceName(),
                                                                                               mbResponce.Header.address,
                                                                                               mbResponce.Header.command,
                                                                                               pSerialBlock->msgNumber);
            break;
         }
         default : printWarning("CxModBusMaster/%s: MB(%s)  unexpected cmd ", __FUNCTION__, getInterfaceName());
      }
   }
   else
   {
      printWarning("CxModBusMaster/%s: MB(%s) cmd was skipped !", __FUNCTION__, getInterfaceName());
   }

   waikeup_by_serial();
}

