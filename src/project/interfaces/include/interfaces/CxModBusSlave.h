#ifndef _CX_MBUS_SLAVE
#define _CX_MBUS_SLAVE

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "common/ptypes.h"
#include "common/utils.h"
#include "os_wrapper/CxThreadIO.h"
#include "interface/CxInterface.h"
#include "serial/CxSerialDriver.h"
#include "MBusDB.h"
//------------------------------------------------------------------------------

class CxModBusSlave : public CxThreadIO, public CxInterface
{
   public:

      CxModBusSlave( const char *interfaceName, const char *drvName );
      virtual ~CxModBusSlave();

      virtual int32_t open  ( );
      virtual int32_t close ( );

   protected :

      virtual void CommandProcessor( uint16_t ComID, void *data );

      void GetRegister( uint16_t reg_start, uint16_t reg_count );
      void SetRegister( uint16_t reg_start, uint16_t reg_value );

   private :

      uint16_t          address;
      TSerialBlock      commbuf;
      TMRESP            mbResponce;

      CxModBusSlave( const CxModBusSlave & );
      CxModBusSlave & operator=( const CxModBusSlave & );
};

//------------------------------------------------------------------------------

#endif // _CX_MBUS_SLAVE

