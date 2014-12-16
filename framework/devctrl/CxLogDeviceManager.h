#ifndef _CX_LOG_DEV_MANAGER
#define _CX_LOG_DEV_MANAGER

//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutex.h"
#include "CxVector.h"
#include "IxLogDevice.h"
//------------------------------------------------------------------------------
#define logDevListSize 20
//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TLogDevListItem
{
  char number;
  IxLogDevice *pLogDevice;
};
#pragma pack ( )
//------------------------------------------------------------------------------

class CxLogDeviceManager
{
 public:

   static CxLogDeviceManager * getInstance();
   void delInstance();

   bool set_logdev( IxLogDevice * pLogDev );
   IxLogDevice *get_logdev( const char *name );

 private:

   CxLogDeviceManager( ); 
   ~CxLogDeviceManager(){}

   CxVector<TLogDevListItem> LOGDEV_LIST;
   uint16_t logDevCounter;
   TLogDevListItem logDevListItemTmp;

   static CxMutex singlDeviceLock;
   static CxLogDeviceManager* theInstance;

   CxLogDeviceManager( CxLogDeviceManager const & other );
   CxLogDeviceManager& operator=( CxLogDeviceManager const & other );

}; typedef CxLogDeviceManager * pCxLogDeviceManager;


#endif /*_CX_LOG_DEV_MANAGER*/   
