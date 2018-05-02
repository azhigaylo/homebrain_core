//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "interfaces/CxModBusSlave.h"
//------------------------------------------------------------------------------

CxModBusSlave::CxModBusSlave( const char *interfaceName, const char *drvName ):
    CxThreadIO     ( interfaceName, drvName )
   ,CxInterface    ( interfaceName )
   ,address        ( 1 )
{
   memset( &commbuf, 0, sizeof commbuf );
}

int32_t CxModBusSlave::open( )
{
   Start();
   return 0;
}

int32_t CxModBusSlave::close( )
{
   printWarning("CxModBusSlave/%s: CxModBusSlave close not implemented", __FUNCTION__);
   return 0;
}

void CxModBusSlave::GetRegister( uint16_t reg_start, uint16_t reg_count )
{
   mbResponce.Header.address = static_cast<uint8_t>(address);
   mbResponce.Header.command = CMD_MB_RREG;
   mbResponce.Header.counter = static_cast<uint8_t>(reg_count * sizeof(uint16_t));

   // prepare data for response
   uint16_t posOut = 0;
   for (uint16_t posIn = reg_start; posIn < reg_start+reg_count; posIn++, posOut++)
   {
     if (posOut < (sizeof(mbResponce.OutputBuf)/2))
     {
        mbResponce.OutputBuf[posOut] = 0;
     }
     else
     {
       printWarning("CxModBusSlave/%s: read register out of range", __FUNCTION__);
     }
   }

   // CRC calculation
   mbResponce.OutputBuf[posOut] = CRC16_T( (char*)&mbResponce, static_cast<uint16_t>(sizeof(TMRESPHeader) + reg_count*sizeof(uint16_t)) );

   commbuf.msgSize   = static_cast<uint16_t>(sizeof(TMRESPHeader) + reg_count*sizeof(uint16_t) + sizeof(uint16_t));
   commbuf.msgNumber = 0;
   memcpy( commbuf.buffer, &mbResponce, commbuf.msgSize);

   // send message to serial driver
   sendMsg( CM_OUT_DATA, &commbuf );
}

void CxModBusSlave::SetRegister( uint16_t /*reg_start*/, uint16_t /*reg_value*/ )
{

}

//------------------------------------------------------------------------------

void CxModBusSlave::CommandProcessor( uint16_t ComID, void *data )
{
   TSerialBlock *pSerialBlock = (TSerialBlock *)data;

   // if we got responce on own ID, we should process it, else - skip it
   switch (ComID)
   {
      case CM_INP_DATA :
      {
         if (0 == CRC16_T(reinterpret_cast<char*>(pSerialBlock->buffer), pSerialBlock->msgSize))
         {
            // copy in the internal buffer(rx buffer can receive another data)
            memcpy( &commbuf, pSerialBlock->buffer, pSerialBlock->msgSize );

            TMREQ *pMREQ = reinterpret_cast<TMREQ*>(commbuf.buffer);

            if (pMREQ->address == address)
            {
               switch (pMREQ->command)
               {
                  case CMD_MB_RREG :
                  {
                     uint16_t start_reg = GenWfrom2B( pMREQ->start_reg_hi, pMREQ->start_reg_low );
                     uint16_t numb_reg  = GenWfrom2B( pMREQ->numb_reg_hi,  pMREQ->numb_reg_low  );

                     GetRegister( start_reg, numb_reg );
                     break;
                  }
                  case CMD_MB_WREG :
                  {
                     TMWRREG *pMWRREG = reinterpret_cast<TMWRREG*>(commbuf.buffer);
                     uint16_t start_reg = GenWfrom2B( pMWRREG->start_reg_hi, pMWRREG->start_reg_low );

                     SetRegister( start_reg, pMWRREG->REG );

                     // send message to serial driver
                     sendMsg( CM_OUT_DATA, &commbuf );

                     break;
                  }
                  case CMD_MB_WARRREG :
                  {
                     printWarning("CxModBusSlave/%s: MB write register block is not supported !", __FUNCTION__ );
                     break;
                  }
                  default : break;
               }


            }
            else
            {
               printDebug("CxModBusSlave/%s: MB address mismatch!", __FUNCTION__ );
            }
         }
         else
         {
            printDebug("CxModBusSlave/%s: MB CRC mismatch!", __FUNCTION__ );
         }
         break;
      }
      case CM_TIMEOUT :
      {
         break;
      }
      default : printWarning("CxModBusSlave/%s: unexpected cmd ", __FUNCTION__);
   }
}

