#ifndef _CX_LOGDEV_DIO_OVEN
#define _CX_LOGDEV_DIO_OVEN

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>
#include <string>

#include "common/ptypes.h"
#include "common/utils.h"
#include "devctrl/CxLogDevice.h"
#include "interfaces/CxModBusMaster.h"
#include "provider/CxDataProvider.h"
//------------------------------------------------------------------------------

#pragma pack(push, 1)

   struct TDioChannel
   {
      int8_t Type;           // chanel type
      uint16_t PointNumb;    // storage point for DI3 chanel
   };// ttl 3 byte

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

      enum TDeviceType
      {
         DT_UNKNOWN  = 0,
         DT_INPUT    = 1,
         DT_OUTPUT   = 2,
         DT_MIXED    = 3
      };

      enum TChType
      {
         CT_UNUSED      = 0,
         CT_DISCRET_IN  = 1,
         CT_DISCRET_OUT = 2
      };

      enum MBusMaskRegister
      {
         discret_output_reg = 0x32,
         discret_input_reg  = 0x33
      };

      TDIO_USO        dev_settings;       // USO settings address and so on
      uint8_t         commError;          // communication error with USO
      CxDataProvider  &dataProvider;      // reference on the data provider
      CxModBusMaster  *pModBusMaster;     // pointer to the interface
      TDeviceType     dev_type;           //

      bool ProcessDevice();
      bool ProcessDiDevice();
      bool ProcessDoDevice();

      TChType GetChannelType( const TDioChannel *pCurCh );
      TDeviceType GetDeviceType( TDIO_USO settings );
      void setUsoStatus( uint16_t status );
      std::string deviceType2Str(TDeviceType d_type);

      CxLogDev_DIO_OVEN( const CxLogDev_DIO_OVEN & );
      CxLogDev_DIO_OVEN & operator=( const CxLogDev_DIO_OVEN & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_DIO_OVEN

