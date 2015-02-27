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

// link record processor
struct TLinkedReg
{
  uint16_t strtReg;
  uint8_t  opType;
  uint16_t NPoint;
};

struct TContExtMod_USO
{
  uint8_t  address;
  uint16_t usoPoint;
  uint16_t recNumb;
  TLinkedReg *pLinkedReg;  // array of the TLinkedReg
};

//------------------------------------------------------------------------------

class CxLogDev_ExtMod : public CxLogDevice, public CxDataProvider, public CxSysTimer
{
   public:

      CxLogDev_ExtMod( const char *logDevName, const char *usedInterface, TContExtMod_USO modSettings );
      ~CxLogDev_ExtMod(){}
      
      virtual void Process();

   protected :

      virtual void sigHandler();

   private :

      TContExtMod_USO  dev_settings;       // USO settings address and so on 
      uint8_t          commError;          // communication error with USO
      CxModBusMaster   *pModBusMaster;     // pointer to the interface

      bool ReadRegisters();
      bool WriteRegisters();

      void setExtModStatus( uint16_t status );

      CxLogDev_ExtMod( const CxLogDev_ExtMod & );
      CxLogDev_ExtMod & operator=( const CxLogDev_ExtMod & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_MA

