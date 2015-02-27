#ifndef _CX_MBUS_MASTER
#define _CX_MBUS_MASTER

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "ptypes.h"
#include "utils.h"
#include "MBusDB.h"
#include "CxThreadIO.h"
#include "CxInterface.h"
#include "CxSerialDriver.h"
#include "IxEventProducer.h"
//------------------------------------------------------------------------------

class CxModBusMaster : public CxThreadIO, public CxInterface, public IxEventProducer
{
   public:

      CxModBusMaster( const char *interfaceName, const char *drvName );
      ~CxModBusMaster(){}

      virtual int32_t open  ( );
      virtual int32_t close ( );

      uint16_t GetRegister( uint8_t address, uint16_t reg_start, uint16_t reg_count, uint16_t *pResponce );  // if ModBusMaster if T_IO_BLOCKED -> false
      bool SetRegister( uint8_t address, uint16_t reg_numb, uint16_t reg_value );

   protected :

      virtual void CommandProcessor( uint16_t ComID, void *data );

   private :

      uint16_t          counter_item;
      uint16_t          sizeResponce; 

      TSerialBlock      commbuf;
      TMREQ             mbReadRequest;
      TMWRREG           mbWriteRequest;
      TMRESP            mbResponce;

      pthread_barrier_t our_barrier;

      CxModBusMaster( const CxModBusMaster & );
      CxModBusMaster & operator=( const CxModBusMaster & );
};

//------------------------------------------------------------------------------

#endif // _CX_MBUS_MASTER

