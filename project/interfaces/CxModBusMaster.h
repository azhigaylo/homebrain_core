#ifndef _CX_MBUS_MASTER
#define _CX_MBUS_MASTER

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "ptypes.h"
#include "utils.h"
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

      bool SetBackEvent( eEventType event );
      void Unblock();

      bool GetRegister( uint16_t reg_start, uint16_t reg_count );  // if ModBusMaster if T_IO_BLOCKED -> false

   protected :
   
      enum TBusMasterState
      {
         ST_MM_FREE = 0,
         ST_IO_WAITFOR_RQ,
         ST_IO_WAITFOR_RESP,
         ST_IO_BLOCKED,
         ST_IO_WAITFOR_UNBLOCK
      };

      virtual void CommandProcessor( uint16_t ComID, void *data );

   private :

      eEventType      backEvent;
      TBusMasterState busMasterState;
      TSerialBlock    commbuf;
      uint32_t        msgIdInProcessing;   // message ID which is processing

      CxModBusMaster( const CxModBusMaster & );
      CxModBusMaster & operator=( const CxModBusMaster & );
};

//------------------------------------------------------------------------------

#endif // _CX_MBUS_MASTER

