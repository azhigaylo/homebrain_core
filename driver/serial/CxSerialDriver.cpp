//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutexLocker.h"
#include "CxSerialDriver.h"
//------------------------------------------------------------------------------

CxSerialDriver::CxSerialDriver( const char *drvName, const char *ttyPath, DCB *pDCB ):
    IxDriver       ( drvName )
   ,CxSysTimer     ( "timeoutTimer", 500000000, false)          // time in nanosecond
   ,internalDCB    ( *pDCB   )
   ,fdTTY          ( 0 )
   ,timeout        ( 0 )
   ,singlSerialLock( "serialLocker" )
{
   memset_m( &tty, 0, sizeof tty, sizeof tty );
   memset_m( &txBuffer, 0, sizeof txBuffer, sizeof txBuffer );
   memset_m( &rxBuffer, 0, sizeof rxBuffer, sizeof rxBuffer );

   ttyConfig( ttyPath );
}

bool CxSerialDriver::ttyConfig(const char *ttyPath)
{
   bool result = false;
   int flags = O_RDWR | O_NOCTTY ;

   fdTTY = open(ttyPath, flags);

   if (-1 == fdTTY)
   {
      printError ("CxSerialDriver/%s: fdTTY=%i, erro=%d", __FUNCTION__, fdTTY, errno);
   }
   else
   {
      printDebug ("CxSerialDriver/%s: fdTTY=%i", __FUNCTION__, fdTTY);
   }

   if (tcgetattr (fdTTY, &tty) == 0)
   {
      cfsetospeed( &tty, internalDCB.BaudRate );
      cfsetispeed( &tty, internalDCB.BaudRate );

      //set bits per byte
      switch (internalDCB.ByteSize)
      {
         case 5  : tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5; break;  // 5-bit chars
         case 6  : tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6; break;  // 6-bit chars
         case 7  : tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7; break;  // 7-bit chars
         case 8  : tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; break;  // 8-bit chars
         default : tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; break;  // 8-bit chars
      }

      //set parity
      switch (internalDCB.Parity)
      {
         case 0  : tty.c_cflag &= ~(PARENB | PARODD);      break;   // no / shut off parity
         case 1  : tty.c_cflag |=  (PARENB | PARODD);      break;   // odd
         case 2  : tty.c_cflag |=   PARENB;                break;   // even
         case 3  : tty.c_cflag |=  (PARENB|PARODD|CMSPAR); break;   // mark parity
         case 4  : tty.c_cflag |=  (PARENB|CMSPAR);        break;   // space parity
         default : tty.c_cflag &= ~(PARENB | PARODD);      break;   // no / shut off parity
      }

      //set StopBits
      switch (internalDCB.StopBits)
      {
         case 1  : tty.c_cflag &= ~CSTOPB;  break;                // 1
         case 2  : tty.c_cflag |=  CSTOPB;  break;                // 2
         default : tty.c_cflag &= ~CSTOPB;  break;                // 1
      }

      tty.c_iflag &= ~(IGNBRK |  ICRNL );                        // disable break processing
      tty.c_lflag = 0;                                           // no signaling chars, no echo, no canonical processing
      tty.c_iflag &= ~(IXON | IXOFF | IXANY);                    // shut off xon/xoff ctrl
      tty.c_cflag |= (CLOCAL | CREAD);                           // ignore modem controls, enable reading
      tty.c_oflag = 0;                                           // no remapping, no delays

      tty.c_cc[VMIN]  = BUFFER_SIZE;   // blocked
      tty.c_cc[VTIME] = 1;             // 0.5 seconds read timeout

      if (tcsetattr (fdTTY, TCSANOW, &tty) != 0)
      {
         printError ("CxSerialDriver/%s: error %d from tcsetattr", __FUNCTION__, errno);
      }
   }
   else
   {
      printError ("CxSerialDriver/%s: error=%d from tcgetattr", __FUNCTION__, errno);
   }

}

CxSerialDriver::~CxSerialDriver()
{
   if (0 != fdTTY)
   {
      close (fdTTY);
   }
}

void CxSerialDriver::startTimer()
{
   StartTimer();
}

void CxSerialDriver::stopTimer()
{
   StopTimer();
}

void CxSerialDriver::CommandProcessor( uint16_t ComID, void *data )
{
   CxMutexLocker locker( &singlSerialLock );

   switch (ComID)
   {
      case CM_SETTIMEOUT :
      {
         if (NULL != data)
         {
            timeout = *(reinterpret_cast<uint32_t*>(data));
            printDebug("CxSerialDriver/%s: timeout=%i ", __FUNCTION__, timeout);
         }
         break;
      }
      case CM_OUT_DATA :
      {
         TSerialBlock* pSerBlk = reinterpret_cast<TSerialBlock*>(data);

         if ((pSerBlk->msgSize > 0)&&(pSerBlk->msgSize < BUFFER_SIZE))
         {
            txBuffer.msgSize   = pSerBlk->msgSize;
            txBuffer.msgNumber = pSerBlk->msgNumber;
            rxBuffer.msgNumber = txBuffer.msgNumber;
            memcpy_m( txBuffer.buffer, pSerBlk->buffer, pSerBlk->msgSize, pSerBlk->msgSize );

            // send to USART
            if (0 != fdTTY)
            {
               if (-1 == write(fdTTY, txBuffer.buffer, txBuffer.msgSize))
               {
                  printWarning("CxSerialDriver/%s: wr for tty=%d error", __FUNCTION__, fdTTY);
               }
               else
               {
                  printDebug("CxSerialDriver/%s: wr to tty=%d, size=%i, package=%i ", __FUNCTION__, fdTTY, txBuffer.msgSize, txBuffer.msgNumber);

                  // start timeout timer
                  startTimer();
               }
            }
         }
         else
         {
            printWarning("CxSerialDriver/%s: buffer overload !", __FUNCTION__);
         }
         break;
      }
      default : break;
   }
}

void CxSerialDriver::ThreadProcessor( )
{
   if (0 != fdTTY)
   {
      uint16_t rxLenght = read( fdTTY, rxBuffer.buffer, sizeof(rxBuffer.buffer) );

      printDebug("CxSerialDriver/%s: tty rec %d byte: %d %d %d %d %d %d %d %d %d %d %d %d %d", __FUNCTION__, rxLenght, rxBuffer.buffer[0],  rxBuffer.buffer[1],  rxBuffer.buffer[2],  rxBuffer.buffer[3],
                                                                                                       rxBuffer.buffer[4],  rxBuffer.buffer[5],  rxBuffer.buffer[6],  rxBuffer.buffer[7],
                                                                                                       rxBuffer.buffer[8],  rxBuffer.buffer[9],  rxBuffer.buffer[10], rxBuffer.buffer[11],
                                                                                                       rxBuffer.buffer[12], rxBuffer.buffer[13]);
      CxMutexLocker locker( &singlSerialLock );

      // stop timeout timer
      stopTimer();

      rxBuffer.msgSize = rxLenght;

      sendMsg( CM_INP_DATA, &rxBuffer );
   }

   sleep_mcs(100);
}

void CxSerialDriver::sigHandler()
{
   CxMutexLocker locker( &singlSerialLock );

   rxBuffer.msgSize = 0;

   sendMsg( CM_TIMEOUT, &rxBuffer );
}

