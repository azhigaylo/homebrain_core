#ifndef _CX_LOG_DEV_MANAGER
#define _CX_LOG_DEV_MANAGER

//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutex.h"
#include "staticpool/CxVector.h"
#include "IxLogDevice.h"
//------------------------------------------------------------------------------
#define logDevListSize 30
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
   static void delInstance();

   bool set_logdev( IxLogDevice * pLogDev );
   IxLogDevice *get_logdev( const char *name );
   IxLogDevice *get_logdev_by_number( uint16_t numb );

   void process_all( );

 private:

   CxLogDeviceManager( );
   ~CxLogDeviceManager(){}

   void clr_logdev_list();

   CxVector<TLogDevListItem> LOGDEV_LIST;
   uint16_t logDevCounter;
   TLogDevListItem logDevListItemTmp;

   static CxMutex singlDeviceLock;
   static CxLogDeviceManager* theInstance;

   CxLogDeviceManager( CxLogDeviceManager const & other );
   CxLogDeviceManager& operator=( CxLogDeviceManager const & other );

}; typedef CxLogDeviceManager * pCxLogDeviceManager;


#endif /*_CX_LOG_DEV_MANAGER*/
