#ifndef _CX_LOGDEV_EXT_MOD
#define _CX_LOGDEV_EXT_MOD

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "ptypes.h"
#include "utils.h"
#include "CxLogDevice.h"
#include "CxModBusMaster.h"
#include "CxDataProvider.h"
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

class CxLogDev_ExtMod : public CxLogDevice, public CxSysTimer
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
      CxDataProvider   &dataProvider;      // reference on the data provider
      CxModBusMaster   *pModBusMaster;     // pointer to the interface

      bool ReadWriteRegisters();
      
      bool getWordToApoint ( const TLinkedReg* pLinkedReg );
      bool getLongToApoint ( const TLinkedReg* pLinkedReg );
      bool getFloatToApoint( const TLinkedReg* pLinkedReg );
      bool getHRegToDpoint ( const TLinkedReg* pLinkedReg );
      bool getLRegToDpoint ( const TLinkedReg* pLinkedReg );

      bool setApointToWord ( const TLinkedReg* pLinkedReg );
      bool setApointToLong ( const TLinkedReg* pLinkedReg );
      bool setApointToFloat( const TLinkedReg* pLinkedReg );
      bool setDpointToReg  ( const TLinkedReg* pLinkedReg );

      void setExtModStatus( uint16_t status );

      CxLogDev_ExtMod( const CxLogDev_ExtMod & );
      CxLogDev_ExtMod & operator=( const CxLogDev_ExtMod & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_EXT_MOD

