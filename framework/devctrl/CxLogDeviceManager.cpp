//------------------------------------------------------------------------------

#include "CxLogDeviceManager.h"

//------------------------------------------------------------------------------

CxLogDeviceManager::CxLogDeviceManager( ):
   LOGDEV_LIST    ( logDevListSize )
  ,logDevCounter  ( 0 )  
{   

}

CxLogDeviceManager &CxLogDeviceManager::getInstance( )
{
  static CxLogDeviceManager theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

bool CxLogDeviceManager::logDevRegistration( pCxLogDevice pLogDevice )
{
  bool result = false;

  // add pointer on logdev in vector
  logDevListItemTmp.number  = logDevCounter;
  logDevListItemTmp.pLogDevice = pLogDevice;
  
  result = LOGDEV_LIST.add( logDevListItemTmp );
  
  // counter increment
  if( true == result )
  {
    logDevCounter++;
  }
  else
  {
    // !!!
  }  
  
  return result;
}

pCxLogDevice CxLogDeviceManager::getLogDev( char number )
{
  if( number < logDevCounter )
  {
    pCxLogDevice pDevice = LOGDEV_LIST[number].pLogDevice;
    
    return pDevice;
  }
  else 
  {
    // !!!
    
    return NULL;
  }  
}

pCxLogDevice CxLogDeviceManager::getLogDev( const char *deviceName )
{
  if( logDevCounter == LOGDEV_LIST.count() )
  {
    for( char itr = 0; itr < logDevCounter; itr++ )
    {
      pCxLogDevice pDevice = LOGDEV_LIST[itr].pLogDevice;
      if( NULL != pDevice )
      {  
        char* devName = pDevice->GetDeviceName( );
        if( true == mod_strcmp( deviceName, devName ) )
        {
          return pDevice;
        }  
      }  
    }
  }  
  
  return NULL;
}

