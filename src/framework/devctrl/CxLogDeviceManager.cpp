//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "interface/CxInterface.h"
#include "os_wrapper/CxMutexLocker.h"
#include "devctrl/CxLogDeviceManager.h"
//------------------------------------------------------------------------------
CxLogDeviceManager* CxLogDeviceManager::theInstance = 0;
CxMutex CxLogDeviceManager::singlDeviceLock("singlDeviceLocker");
//------------------------------------------------------------------------------

CxLogDeviceManager::CxLogDeviceManager( ):
    LOGDEV_LIST ( )
{

}

CxLogDeviceManager * CxLogDeviceManager::getInstance( )
{
   if(CxLogDeviceManager::theInstance == 0)
   {
      CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

      if(CxLogDeviceManager::theInstance == 0)
      {
         CxLogDeviceManager::theInstance = new CxLogDeviceManager();
      }
   }

  return theInstance;
}

void CxLogDeviceManager::delInstance()
{
   if(CxLogDeviceManager::theInstance != 0)
   {
      CxLogDeviceManager::theInstance->clr_logdev_list();
      // remove singleton item
      delete CxLogDeviceManager::theInstance;
      printDebug("CxLogDeviceManager/%s: instance deleted", __FUNCTION__);
   }
}

//------------------------------------------------------------------------------

bool CxLogDeviceManager::set_logdev( IxLogDevice * pLogDev )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   // add pointer on logdev in vector
   logDevListItemTmp.number  = LOGDEV_LIST.size();
   logDevListItemTmp.pLogDevice = pLogDev;

   LOGDEV_LIST.push_back( logDevListItemTmp );

   // counter increment
   printDebug("CxLogDeviceManager/%s: LogDev = %s registred ", __FUNCTION__, pLogDev->getDeviceName());

   return true;
}

IxLogDevice * CxLogDeviceManager::get_logdev( const char *name )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   for( uint8_t itr = 0; itr < LOGDEV_LIST.size(); itr++ )
   {
      IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;
      if( NULL != pDevice )
      {
         const char* devName = pDevice->getDeviceName( );
         if( 0 == strcmp( name, devName ) )
         {
            return pDevice;
         }
      }
   }

   return 0;
}

IxLogDevice *CxLogDeviceManager::get_logdev_by_number( uint16_t numb )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   if (numb < LOGDEV_LIST.size())
   {
      IxLogDevice *pDevice = LOGDEV_LIST[numb].pLogDevice;

      return pDevice;
   }

   return 0;
}

void CxLogDeviceManager::clr_logdev_list()
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   for( uint8_t itr = 0; itr < LOGDEV_LIST.size(); itr++ )
   {
      IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;
      if( NULL != pDevice )
      {
         printDebug("CxLogDeviceManager/%s: try to remove LogDev = %s", __FUNCTION__, pDevice->getDeviceName());
         delete pDevice;
      }
   }

   // clean up list
   LOGDEV_LIST.clear();
}

void CxLogDeviceManager::process_all( )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   for( uint8_t itr = 0; itr < LOGDEV_LIST.size(); itr++ )
   {
      IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;

      if( NULL != pDevice )
      {
         pDevice->Process();
      }
   }

}
