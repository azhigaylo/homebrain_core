#ifndef _CX_LOGDEV_MA
#define _CX_LOGDEV_MA

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "ptypes.h"
#include "utils.h"
#include "CxLogDevice.h"
#include "CxDataProvider.h"
#include "CxModBusMaster.h"
//------------------------------------------------------------------------------

struct TAioChannel
{
   uint8_t  Number;          // number of channel
   uint8_t  Type;            // channel type
   float    MinMid;          // low limit of middle value
   float    MaxMid;          // hi limit of middle value
   float    MinVal;          // low limit of physical value
   float    MaxVal;          // hi limit of physical value
   uint16_t Code;            // code from ADC
   float    MidValue;        // middle value
   float    PhisValue;       // physical value
   uint8_t  ChanelStatus;    // status
   uint16_t MidPointNumb;    // storage point for middle value
   uint16_t ValPointNumb;    // storage point for AI & DI3 channel
}; // ttl 29 byte

struct TContAI_USO
{
   uint8_t  address;  
   uint16_t usoPoint;
   uint8_t  chanNumb;
   TAioChannel *channelsPtr;  // array of the TAioChannel
}; // ttl 5 byte

//------------------------------------------------------------------------------

class CxLogDev_MA : public CxLogDevice, public CxDataProvider, public CxSysTimer
{
   public:

      CxLogDev_MA( const char *logDevName, const char *usedInterface, TContAI_USO usoSettings );
      ~CxLogDev_MA(){}
      
      virtual void Process();

   protected :

      virtual void sigHandler();

   private :

      enum TChType
      {
         CT_UNUSED      = 0,
         CT_ANALOG_IN   = 1,
         CT_ANALOG_OUT  = 2,
         CT_DISCRET_IN  = 3,
         CT_DISCRET_OUT = 4
      };   
   
      TContAI_USO    dev_settings;       // USO settings address and so on 
      uint8_t        commError;          // communication error with USO
      CxModBusMaster *pModBusMaster;     // pointer to the interface

      bool CheckAndSetOutput();
      bool ReadRegisters();

      TChType GetChannelType( const TAioChannel *pCurCh  );
      void setUsoStatus( uint16_t status );
      void ConvAiToVal();
      void ConvAiToParam();

      CxLogDev_MA( const CxLogDev_MA & );
      CxLogDev_MA & operator=( const CxLogDev_MA & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_MA

