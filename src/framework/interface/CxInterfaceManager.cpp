//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutexLocker.h"
#include "interface/CxInterface.h"
#include "interface/CxInterfaceManager.h"

//------------------------------------------------------------------------------
CxInterfaceManager* CxInterfaceManager::theInstance = 0;
CxMutex CxInterfaceManager::singlInterfaceLock("singlInterfaceLocker");
//------------------------------------------------------------------------------

CxInterfaceManager::CxInterfaceManager( ):
   interfaceCounter( 0 )
  ,CONNECTION_LIST ( interfaceListSize )
{

}

CxInterfaceManager * CxInterfaceManager::getInstance( )
{
   if(theInstance == 0)
   {
      CxMutexLocker locker(&CxInterfaceManager::singlInterfaceLock);

      if(theInstance == 0)
      {
         theInstance = new CxInterfaceManager;
      }
   }

  return theInstance;
}

void CxInterfaceManager::delInstance()
{
   if(CxInterfaceManager::theInstance != 0)
   {
      CxMutexLocker locker(&CxInterfaceManager::singlInterfaceLock);

      for( uint8_t itr = 0; itr < CONNECTION_LIST.count(); itr++ )
      {
         pIxInterface pInterface = CONNECTION_LIST[itr].pInterface;

         if (pInterface != 0)
         {
            printDebug("CxInterfaceManager/%s: try to remove interface = %s...", __FUNCTION__, pInterface->getInterfaceName());
            delete pInterface;
         }
      }
      // clean up list
      CONNECTION_LIST.clear();
      // remove singleton item
      delete this;
   }
}

bool CxInterfaceManager::set_interface( IxInterface * pNewInterface )
{
   bool result = false;

   CxMutexLocker locker(&CxInterfaceManager::singlInterfaceLock);

   TInterfaceItem connectionItem;

   connectionItem.ID = static_cast<uint16_t>(rand());
   connectionItem.pInterface = pNewInterface;
   result = CONNECTION_LIST.add( connectionItem );

   interfaceCounter++;

   printDebug("CxInterfaceManager/%s: Interface = %s registred ", __FUNCTION__, pNewInterface->getInterfaceName());

   return result;
}

IxInterface *CxInterfaceManager::get_interface( const char *name )
{
   CxMutexLocker locker(&CxInterfaceManager::singlInterfaceLock);

   for( uint8_t itr = 0; itr < CONNECTION_LIST.count(); itr++ )
   {
      pIxInterface pInterface = CONNECTION_LIST[itr].pInterface;
 
      if (pInterface != 0)
      {
         const char *nameInterf = pInterface->getInterfaceName();

         if( 0 == strcmp( name, nameInterf ) )
         { 
           return pInterface;
         }
      }
   }
   return 0;
}
