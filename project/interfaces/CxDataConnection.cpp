//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "slog.h"
#include "utils.h"
#include "CxDataConnection.h"
#include "CxDataProvider.h"
//------------------------------------------------------------------------------

#define SOCKET_INVALID -1
#define QSTRING_NULL 0xFFFFFFFF

// Swap 2 byte, 16 bit values:
#define Swap2Bytes(val) \
 ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

// Swap 4 byte, 32 bit values:
#define Swap4Bytes(val) \
 ( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )
 
 // Swap 8 byte, 64 bit values:
#define Swap8Bytes(val) \
 ( (((val) >> 56) & 0x00000000000000FF) | (((val) >>  40) & 0x000000000000FF00) | \
   (((val) >> 24) & 0x0000000000FF0000) | (((val) >>   8) & 0x00000000FF000000) | \
   (((val) <<  8) & 0x000000FF00000000) | (((val) <<  24) & 0x0000FF0000000000) | \
   (((val) << 40) & 0x00FF000000000000) | (((val) <<  56) & 0xFF0000000000000) )
  
#define TtNotificationSet   1
#define TtNotificationReset 2
#define TtDataUpdate        3
#define TtDataGet           4
#define TtCommand           5
#define TtServiceInfo       6

//------------------------------------------------------------------------------

const char* APP_NAME = "HomeBrain";

//------------------------------------------------------------------------------

CxDataConnection::CxDataConnection( const char *pInterfaceName, const char *strServer ) :
    IxRunnable  ( pInterfaceName )
   ,CxInterface ( pInterfaceName )
   ,mSocket     ( 0 )
   ,reconnectFlag    ( 0 )
   ,notifiArraySize  ( 0 )
   ,pNotifiItemArray ( 0 )
{
   int32_t rv;

   // create notification array
   createNotifiItemArray();
     
   // config sockaddr_in
   char *strServer_port = strdup(strServer);
   char *strServer_ip = strsep(&strServer_port, ":");

   struct addrinfo hints = {0};
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   if ((rv = getaddrinfo(strServer_ip, strServer_port, &hints, &servinfo)) != 0) 
   {
      printWarning("CxDataConnection/%s: getaddrinfo error=%s", __FUNCTION__, gai_strerror(rv));
   }
   else
   {
      printDebug("CxDataConnection/%s: connection ip/port = %s/%i", __FUNCTION__, strServer_ip, atoi(strServer_port));
   }

   setReconnectFlag();
}

int32_t CxDataConnection::open( )
{
   task_run();
}

int32_t CxDataConnection::close( )
{
   printWarning("CxDataConnection/%s: CxDataConnection close not implemented", __FUNCTION__);
}

CxDataConnection::~CxDataConnection( void )
{
   if (pNotifiItemArray != 0)
   {
      delete[] pNotifiItemArray;
   }
   // close socket
   disconnectSocket();
   // No longer needed
   freeaddrinfo(servinfo);
}

//------------------------------------------------------------------------------

bool CxDataConnection::connectSocket()
{
   bool result = false;

   mSocket = socket(AF_INET, SOCK_STREAM, 0); // TCP-server creation

   if(mSocket < 0 || servinfo == 0)
   {
      printError("CxDataConnection/%s: socket error", __FUNCTION__);
   }

   if(0 != connect(mSocket,  servinfo->ai_addr, servinfo->ai_addrlen) < 0)
   {
      printError("CxDataConnection/%s: socket connection error", __FUNCTION__);
   }
   else
   {
      printDebug("CxDataConnection/%s: socket connected", __FUNCTION__);

      // we should clear all notifications, because connection on the server was renewed
      clrNotifiItemArray();
      // set MY NAME on the server 
      setString( APP_NAME );
      
      result = true;
   }

   return result;
}

void CxDataConnection::disconnectSocket()
{
   do
   {
      if (SOCKET_INVALID == mSocket)
      {
         break;
      }

      if (0 != shutdown(mSocket, SHUT_RDWR))
      {
         printError("CxDataConnection/%s: Unable to shutdown socket: %s", __FUNCTION__, strerror(errno));
         // Do not break - the socket needs to be closed
      } else
      {
         printDebug("CxDataConnection/%s: Disconnected server...", __FUNCTION__); 
      }

      if (0 != ::close(mSocket))
      {
         printError("CxDataConnection/%s: Unable to close socket: %s", __FUNCTION__, strerror(errno));
         break;
      }
   } while (0);

   mSocket = SOCKET_INVALID;
}

void CxDataConnection::clrNotifiItemArray()
{
   if (0 != pNotifiItemArray)
   {
      for (uint32_t itm = 0; itm < notifiArraySize; itm++)
      {
         TNotifiItem dummyNotifiItem = {0,0,0};
         pNotifiItemArray[itm] = dummyNotifiItem;
      }
   }
}

void CxDataConnection::createNotifiItemArray()
{
   // calculate size
   if ( d_point_total >= a_point_total ) notifiArraySize = d_point_total;
   else notifiArraySize = a_point_total;
   
   // create array
   pNotifiItemArray = new TNotifiItem[notifiArraySize];
}

bool CxDataConnection::doesItNotifiable(uint32_t number, uint32_t type)
{
   bool result = false;

   if ((pNotifiItemArray != 0) && (notifiArraySize >= number))
   {
      switch (type)
      {
         case DtDigitalPoint :
         {
            if (pNotifiItemArray[number].nDpNotifi == 0)
            {
               pNotifiItemArray[number].nDpNotifi = 1;
               result = true;
            }
            break;
         }
         case DtAnalogPoint :
         {
            if (pNotifiItemArray[number].nApNotifi == 0)
            {
               pNotifiItemArray[number].nApNotifi = 1;
               result = true;
            }
            break;
         }
         default : break;
      }
   }

   printDebug("CxDataConnection/%s: number=%d / type=%d : result =%d", __FUNCTION__, number, type, result);

   return result;
}

//------------------------------------------------------------------------------

void CxDataConnection::TaskProcessor()
{  
   if (true == getReconnectFlag())
   {
      if (true == connectSocket())
      {
         resetReconnectFlag();
         printDebug("CxDataConnection/%s: reconnected...", __FUNCTION__);
         sendEvent( event_pool::EVENT_DATA_CONNECTED, 0, 0 );
      }
   }

   if (SOCKET_INVALID != mSocket )
   {
      uint32_t expectedSize = 0;
      const ssize_t size = recv(mSocket, reinterpret_cast<char*>(&expectedSize), sizeof expectedSize, 0);
      expectedSize = Swap4Bytes( expectedSize );
      
      if (0 >= size)
      {
         printWarning("CxDataConnection/%s: Unable to receive data: %s, trying to reconnect...", __FUNCTION__, strerror(errno));
         setReconnectFlag();
      }
      else
      {
         TQByteArray qByteArray;
         const ssize_t size = recv(mSocket, reinterpret_cast<char*>(&qByteArray), expectedSize + sizeof(qByteArray.byteArrayBody), 0);

         if (0 >= size)
         {
            printWarning("CxDataConnection/%s: Unable to receive data: %s, trying to reconnect...", __FUNCTION__, strerror(errno)); 
            setReconnectFlag();
            return;
         }
         
         printDebug("CxDataConnection/%s: rec size= %i/ expected= %i", __FUNCTION__, size, expectedSize);

         if (0 != CRC16_T(reinterpret_cast<char*>(&qByteArray.tradeUnit), size - sizeof(qByteArray.byteArrayBody)))
         {
            printWarning("CxDataConnection/%s: NOK CRC", __FUNCTION__);
         }
         else
         {
            switch( Swap4Bytes(qByteArray.tradeUnit.tradeType) )
            {
               case TtDataUpdate         :
               {
                  switch( Swap4Bytes(qByteArray.tradeUnit.dataType) )
                  {
                     case DtDigitalPoint :
                     {
                        const TQDpoint *pDpoint = reinterpret_cast<TQDpoint*>(qByteArray.Array);
                        onDataUpdate( Swap4Bytes(qByteArray.tradeUnit.dataNumber), Swap4Bytes(pDpoint->status), Swap4Bytes(pDpoint->value) );
                        break;
                     }
                     case DtAnalogPoint  :
                     case DtStringPoint  :
                     case DtStringVector :
                     default :
                     {
                        printWarning("CxDataConnection/%s: this dataType doesn't supported ", __FUNCTION__);
                        break;
                     }
                  }
                 break;
               }
               case TtDataGet:
               case TtCommand:
               case TtServiceInfo:
               default :
               {
                  printWarning("CxDataConnection/%s: unresolved tcp package", __FUNCTION__);
                  break;
               }
            }
         }
      }
   }
}

bool CxDataConnection::setNotification(uint32_t number, uint32_t type)
{
   bool ret = false;

   TQByteArray byteArray;

   if ( true == doesItNotifiable(number, type))
   {
      // fill out TQByteArray buffer
      byteArray.tradeUnit.tradeType  = Swap4Bytes( TtNotificationSet );
      byteArray.tradeUnit.dataType   = Swap4Bytes( type );
      byteArray.tradeUnit.dataNumber = Swap4Bytes( number );

      uint16_t crc = CRC16_T(reinterpret_cast<char*>(&byteArray.tradeUnit), sizeof(byteArray.tradeUnit));

      memcpy(byteArray.Array, reinterpret_cast<char*>(&crc), sizeof(crc));

      byteArray.byteArrayBody = sizeof(byteArray.tradeUnit) + sizeof(crc);  

      uint32_t sendSize     = sizeof(byteArray.byteArrayBody) + byteArray.byteArrayBody;   
      uint32_t expectedSize = Swap4Bytes(byteArray.byteArrayBody);

      byteArray.byteArrayBody = Swap4Bytes( byteArray.byteArrayBody );

      //---send io buffer---
                   send( mSocket, &expectedSize, sizeof(expectedSize), MSG_DONTWAIT );
      ssize_t rc = send( mSocket, &byteArray,    sendSize,             MSG_DONTWAIT );

      if (-1 == rc or static_cast<size_t>(rc) != sendSize)
      {
         printError("CxDataConnection/%s: Unable to send: %s, trying to reconnect...", __FUNCTION__, strerror(errno)); 
         setReconnectFlag();
         ret = false;
      }
      else
      {
         ret = true;
      }
   }
   return ret;
}

bool CxDataConnection::setApoint(uint32_t number, uint32_t status, float value)
{
   bool     ret = false;
   double   inputMetaValue = value;

   uint64_t inputValue = 0;
   memcpy(reinterpret_cast<char*>(&inputValue), reinterpret_cast<char*>(&inputMetaValue), sizeof(inputValue));

   TQByteArray byteArray;
   TQApoint aPoint = { Swap4Bytes(status), Swap8Bytes(inputValue) };
   //---------------------------- 

   // fill out TQByteArray buffer
   byteArray.tradeUnit.tradeType  = Swap4Bytes( TtDataUpdate );
   byteArray.tradeUnit.dataType   = Swap4Bytes( DtAnalogPoint );
   byteArray.tradeUnit.dataNumber = Swap4Bytes( number );

   memcpy(byteArray.Array, reinterpret_cast<char*>(&aPoint), sizeof(aPoint));

   uint16_t crc = CRC16_T(reinterpret_cast<char*>(&byteArray.tradeUnit), sizeof(byteArray.tradeUnit) + sizeof(aPoint));

   memcpy(byteArray.Array + sizeof(aPoint), reinterpret_cast<char*>(&crc), sizeof(crc));

   byteArray.byteArrayBody = sizeof(byteArray.tradeUnit) + sizeof(aPoint) + sizeof(crc);

   uint32_t sendSize     = sizeof(byteArray.byteArrayBody) + byteArray.byteArrayBody;
   uint32_t expectedSize = Swap4Bytes(byteArray.byteArrayBody);

   byteArray.byteArrayBody = Swap4Bytes( byteArray.byteArrayBody );

   //---send io buffer---
                send( mSocket, &expectedSize, sizeof(expectedSize), MSG_DONTWAIT );
   ssize_t rc = send( mSocket, &byteArray,    sendSize,             MSG_DONTWAIT );

   if (-1 == rc or static_cast<size_t>(rc) != sendSize)
   {
      printError("CxDataConnection/%s: Unable to send: %s, trying to reconnect...", __FUNCTION__, strerror(errno)); 
      setReconnectFlag();
      ret = false;
   }
   else
   {
      ret = true;
   }

   return ret;
}

bool CxDataConnection::setDpoint(uint32_t number, uint32_t status, uint32_t value)
{
   bool ret = false;

   TQByteArray byteArray;
   TQDpoint dPoint = {Swap4Bytes(status), Swap4Bytes(value)};

   // fill out TQByteArray buffer
   byteArray.tradeUnit.tradeType  = Swap4Bytes( TtDataUpdate );
   byteArray.tradeUnit.dataType   = Swap4Bytes( DtDigitalPoint );
   byteArray.tradeUnit.dataNumber = Swap4Bytes( number );

   memcpy(byteArray.Array, reinterpret_cast<char*>(&dPoint), sizeof(dPoint));

   uint16_t crc = CRC16_T(reinterpret_cast<char*>(&byteArray.tradeUnit), sizeof(byteArray.tradeUnit) + sizeof(dPoint));

   memcpy(byteArray.Array + sizeof(dPoint), reinterpret_cast<char*>(&crc), sizeof(crc));

   byteArray.byteArrayBody = sizeof(byteArray.tradeUnit) + sizeof(dPoint) + sizeof(crc);

   uint32_t sendSize     = sizeof(byteArray.byteArrayBody) + byteArray.byteArrayBody;
   uint32_t expectedSize = Swap4Bytes(byteArray.byteArrayBody);

   byteArray.byteArrayBody = Swap4Bytes( byteArray.byteArrayBody );

   //---send io buffer---
                send( mSocket, &expectedSize, sizeof(expectedSize), MSG_DONTWAIT );
   ssize_t rc = send( mSocket, &byteArray,    sendSize,             MSG_DONTWAIT );

   if (-1 == rc or static_cast<size_t>(rc) != sendSize)
   {
      printError("CxDataConnection/%s: Unable to send: %s, trying to reconnect...", __FUNCTION__, strerror(errno));
      setReconnectFlag();
      ret = false;
   }
   else
   {
      ret = true;
   }

   return ret;
}

bool CxDataConnection::setString(const char* const pString)
{
   bool ret = false;

   TQByteArray byteArray;
   TQString    qString;
   uint16_t    str_size  = 0;

   // fill out TQString buffer
   if (0 != pString)
   {
      str_size = strlen(pString) * 2;                        // Body text in UTF16

      qString.SrtingBody = Swap4Bytes(str_size);

      for (size_t i = 0; str_size / 2 > i; ++i)
      {
         qString.String[i] = GenWfrom2B( static_cast<uint8_t>(pString[i]), 0 );
      }
   }

   // fill out TQByteArray buffer
   byteArray.tradeUnit.tradeType  = Swap4Bytes(TtServiceInfo);
   byteArray.tradeUnit.dataType   = Swap4Bytes(DtStringPoint);
   byteArray.tradeUnit.dataNumber = 0;

   memcpy(byteArray.Array, reinterpret_cast<char*>(&qString), sizeof(qString.SrtingBody) + str_size);

   uint16_t crc = CRC16_T(reinterpret_cast<char*>(&byteArray.tradeUnit), sizeof(byteArray.tradeUnit) + sizeof(qString.SrtingBody) + str_size);

   memcpy(byteArray.Array + sizeof(qString.SrtingBody) + str_size, reinterpret_cast<char*>(&crc), sizeof(crc));

   byteArray.byteArrayBody = sizeof(byteArray.tradeUnit) + sizeof(qString.SrtingBody) + str_size + sizeof(crc);

   uint32_t sendSize     = sizeof(byteArray.byteArrayBody) + byteArray.byteArrayBody;
   uint32_t expectedSize = Swap4Bytes(byteArray.byteArrayBody);

   byteArray.byteArrayBody = Swap4Bytes( byteArray.byteArrayBody );

   //---send io buffer---
                send( mSocket, &expectedSize, sizeof(expectedSize), MSG_DONTWAIT );
   ssize_t rc = send( mSocket, &byteArray,    sendSize,             MSG_DONTWAIT );

   if (-1 == rc or static_cast<size_t>(rc) != sendSize)
   {
      printError("CxDataConnection/%s: Unable to send: %s, trying to reconnect...", __FUNCTION__, strerror(errno));
      setReconnectFlag();
      ret = false;
   }
   else
   {
      ret = true;
   }

   return ret;
}

bool CxDataConnection::onDataUpdate( uint32_t number, uint32_t status, uint32_t value )
{
   printDebug("CxDataConnection/%s: got new dpoint number/status/value = %d/%d/%d", __FUNCTION__, number, status, value);
   
   CxDataProvider::getInstance().setSilenceDPoint( number, value );
   CxDataProvider::getInstance().setSilenceDStatus( number, status );
}
