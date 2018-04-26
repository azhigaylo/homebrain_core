#ifndef _CX_DATA_CONNECTION
#define _CX_DATA_CONNECTION

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <streambuf>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ptypes.h"
#include "IxRunnable.h"
#include "CxInterface.h"
#include "IxEventProducer.h"
//------------------------------------------------------------------------------

#define DtDigitalPoint 1
#define DtAnalogPoint  2
#define DtStringPoint  3
#define DtStringVector 4

//------------------------------------------------------------------------------
struct TNotifiItem
{
   uint8_t nDpNotifi  : 1;    // 1 bits
   uint8_t nApNotifi  : 1;    // 1 bits
   uint8_t xxx        : 6;    // 6 bits
};


#pragma pack(push, 1)

   struct TTradeUnit
   {
      uint32_t tradeType;
      uint32_t dataType;
      uint32_t dataNumber;
   };

   struct TQDpoint
   {
      uint32_t status;
      uint32_t value;
   };

   struct TQApoint
   {
     uint32_t status;
     uint64_t value;
   };

   struct TQByteArray
   {
      uint32_t   byteArrayBody;
      TTradeUnit tradeUnit;
      int8_t     Array[400];
   };

   struct TQString
   {
      uint32_t   SrtingBody;
      uint16_t   String[200];
   };

#pragma pack(pop)
//------------------------------------------------------------------------------

class CxDataConnection : public IxRunnable, public CxInterface, public IxEventProducer
{
   public:

      CxDataConnection( const char *pInterfaceName, const char *strServer );
      virtual ~CxDataConnection( );

      virtual int32_t open  ( );
      virtual int32_t close ( );

      bool setNotification( uint32_t number, uint32_t type );
      bool setDpoint( uint32_t number, uint32_t status, uint32_t value );
      bool setApoint( uint32_t number, uint32_t status, float value );
      bool setString( const char* const pString );

   protected:

      virtual void TaskProcessor();
      bool onDataUpdate( uint32_t number, uint32_t status, uint32_t value );

private:

     void setReconnectFlag(){reconnectFlag = true;}
     void resetReconnectFlag(){reconnectFlag = false;}
     bool getReconnectFlag(){return reconnectFlag;}
     bool connectSocket();
     void disconnectSocket();
     void clrNotifiItemArray();
     void createNotifiItemArray();
     bool doesItNotifiable(uint32_t number, uint32_t type);

     struct addrinfo *servinfo;
     int mSocket;                       // socket descriptor
     bool reconnectFlag;                // if true - trying to reconnect
     uint32_t notifiArraySize;
     TNotifiItem *pNotifiItemArray;     // pointer on the notification array
};

//-------------------------------------------------------------------

#endif // CX_CONNECTION
