//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxInterface.h"
#include "CxMutexLocker.h"
#include "CxLogDeviceManager.h"
//------------------------------------------------------------------------------
CxLogDeviceManager* CxLogDeviceManager::theInstance = 0;
CxMutex CxLogDeviceManager::singlDeviceLock("singlDeviceLocker");
//------------------------------------------------------------------------------

CxLogDeviceManager::CxLogDeviceManager( ):
    LOGDEV_LIST   ( logDevListSize )
   ,logDevCounter ( 0 )
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
      if( logDevCounter == LOGDEV_LIST.count() )
      {
         CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

         for( uint8_t itr = 0; itr < logDevCounter; itr++ )
         {
            IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;
            if( NULL != pDevice )
            {
               printDebug("CxLogDeviceManager/%s: try to remove LogDev = %s...", __FUNCTION__, pDevice->getDeviceName());
               delete pDevice;
            }
         }
      }
      // clean up list
      LOGDEV_LIST.clear();
      // remove singleton item
      delete this;
   }
}

//------------------------------------------------------------------------------

bool CxLogDeviceManager::set_logdev( IxLogDevice * pLogDev )
{
   bool result = false;

   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   // add pointer on logdev in vector
   logDevListItemTmp.number  = logDevCounter;
   logDevListItemTmp.pLogDevice = pLogDev;

   result = LOGDEV_LIST.add( logDevListItemTmp );

   // counter increment
   if( true == result )
   {
     logDevCounter++;
     printDebug("CxLogDeviceManager/%s: LogDev = %s registred ", __FUNCTION__, pLogDev->getDeviceName());
   }
   else
   {
     printError("CxLogDeviceManager/%s: LogDev = %s registartion error ", __FUNCTION__, pLogDev->getDeviceName());
   }  

   return result;
}

IxLogDevice * CxLogDeviceManager::get_logdev( const char *name )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   if( logDevCounter == LOGDEV_LIST.count() )
   {
      for( uint8_t itr = 0; itr < logDevCounter; itr++ )
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
   }  

  return 0;
}

IxLogDevice *CxLogDeviceManager::get_logdev_by_number( uint16_t numb )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   if( logDevCounter == LOGDEV_LIST.count() )
   {
      if (numb < logDevCounter)
      {
         IxLogDevice *pDevice = LOGDEV_LIST[numb].pLogDevice;

         return pDevice;
      }
   }

  return 0;
}

void CxLogDeviceManager::process_all( )
{
   CxMutexLocker locker(&CxLogDeviceManager::singlDeviceLock);

   if( logDevCounter == LOGDEV_LIST.count() )
   {
      for( uint8_t itr = 0; itr < logDevCounter; itr++ )
      {
         IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;

         if( NULL != pDevice )
         {
            pDevice->Process();
         }
      }
   } 
}
