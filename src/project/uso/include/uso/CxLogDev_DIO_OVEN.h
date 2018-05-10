#ifndef _CX_LOGDEV_DIO_OVEN
#define _CX_LOGDEV_DIO_OVEN

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "common/ptypes.h"
#include "common/utils.h"
#include "devctrl/CxLogDevice.h"
#include "interfaces/CxModBusMaster.h"
#include "provider/CxDataProvider.h"
//------------------------------------------------------------------------------

#pragma pack(push, 1)

   struct TDioChannel
   {
      uint8_t  Number;          // number of channel
      uint8_t  Type;            // channel type
      uint16_t Code;            // raw data from register
   };

   struct TDIO_USO
   {
      uint8_t  address;
      uint16_t usoPoint;
      uint8_t  chanNumb;
      TDioChannel *channelsPtr;  // array of the TAioChannel
   }; // ttl 5 byte

#pragma pack(pop)
//------------------------------------------------------------------------------

class CxLogDev_DIO_OVEN : public CxLogDevice, public CxSysTimer
{
   public:

      CxLogDev_DIO_OVEN( const char *logDevName, const char *usedInterface, TDIO_USO usoSettings );
      virtual ~CxLogDev_DIO_OVEN();

      virtual bool Process();

   protected :

      virtual void sigHandler();

   private :

      enum TChType
      {
         CT_UNUSED      = 0,
         CT_DISCRET_IN  = 1,
         CT_DISCRET_OUT = 2
      };

      TDIO_USO        dev_settings;       // USO settings address and so on
      uint8_t         commError;          // communication error with USO
      CxDataProvider  &dataProvider;      // reference on the data provider
      CxModBusMaster  *pModBusMaster;     // pointer to the interface

      bool CheckAndSetOutput();
      bool ReadRegisters();

      TChType GetChannelType( const TDioChannel *pCurCh  );
      void setUsoStatus( uint16_t status );

      CxLogDev_DIO_OVEN( const CxLogDev_DIO_OVEN & );
      CxLogDev_DIO_OVEN & operator=( const CxLogDev_DIO_OVEN & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_DIO_OVEN

