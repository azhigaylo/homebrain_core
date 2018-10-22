#ifndef _CX_SERIAL_DRIVER
#define _CX_SERIAL_DRIVER

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "common/ptypes.h"
#include "common/utils.h"
#include "os_wrapper/CxMutex.h"
#include "os_wrapper/CxSysTimer.h"
#include "os_wrapper/IxDriver.h"
//------------------------------------------------------------------------------

// command ID
// 1 and 2 already captured for IxDriver
#define CM_SETTIMEOUT 3
#define CM_OUT_DATA   4
#define CM_INP_DATA   5
#define CM_TIMEOUT    6
//------------------------------------------------------------------------------
#define BUFFER_SIZE 500
//------------------------------------------------------------------------------

struct DCB
{
  uint32_t BaudRate;                                    // current baud rate
  uint8_t  Parity;                                      // 0-4=no,odd,even,mark,space
  uint8_t  ByteSize;                                    // number of bits/byte (5-8)
  uint8_t  StopBits;                                    // 1,2
};

struct TSerialBlock
{
  uint16_t msgSize;
  uint32_t msgNumber;
  uint8_t  buffer[BUFFER_SIZE];
};
//------------------------------------------------------------------------------
class CxSerialDriver : public IxDriver, public CxSysTimer
{
   public:

      // we hide it because everybody should inherit it !
      CxSerialDriver( const char *drvName, const char *ttyPath, DCB *pDCB );
      ~CxSerialDriver();

   protected :

      void stopTimer();
      void startTimer();

   private :

      DCB internalDCB;
      int fdTTY;
      struct termios tty;
      uint32_t timeout;
      char *my_ttyPath;

      TSerialBlock rxBuffer;
      TSerialBlock txBuffer;

      CxMutex singlSerialLock;

      virtual void CommandProcessor( uint16_t ComID, void *data );
      virtual void ThreadProcessor ( );
      virtual void sigHandler();

      bool ttyConfig( const char *ttyPath );

      CxSerialDriver( const CxSerialDriver & );
      CxSerialDriver & operator=( const CxSerialDriver & );
};

//------------------------------------------------------------------------------

#endif // _CX_SERIAL_DRIVER

