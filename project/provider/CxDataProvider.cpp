//------------------------------------------------------------------------------
#include "utils.h"
#include "DebugMacros.h"
#include "CxMutexLocker.h"
#include "CxDataProvider.h"
#include "CxInterfaceManager.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//--------------------------------data provider class---------------------------
CxMutex CxDataProvider::digitalDataProviderMutex("d_data_mutex");
CxMutex CxDataProvider::analogDataProviderMutex("a_data_mutex");
//------------------------------------------------------------------------------

CxDataProvider::CxDataProvider() :
    pDataConnection( 0 )
{
   // clearing discret point
   for( uint8_t point=0; point < d_point_total; point++ )
   {
      DPOINT[point].number = point;
      DPOINT[point].status = 0;
      DPOINT[point].value = 0;
   }
   // clearing analog point  
   for( uint8_t point=0; point < a_point_total; point++ )
   {
      APOINT[point].number = point;
      APOINT[point].status = 0;  
      APOINT[point].value = 0;
   }
} 

CxDataProvider::~CxDataProvider()
{

}

CxDataProvider &CxDataProvider::getInstance()
{
   static CxDataProvider theInstance;
   return theInstance;
}

//------------------------------------------------------------------------------

CxDataConnection *CxDataProvider::getInterface()
{
   if (0 == pDataConnection)
   {
      pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
      pIxInterface pInterface = pInterfaceManager->get_interface( "dataInterface" );
      pDataConnection = dynamic_cast<CxDataConnection*>(pInterface);

      printDebug("CxDataConnection/%s: pDataConnection=%i", __FUNCTION__, pDataConnection);
   }
   
   return pDataConnection;
}
      
void CxDataProvider::sendExternalDpoint( uint16_t number )
{
   // put in to external storage
   CxDataConnection *pInterface = getInterface();  
   if (pInterface != 0)
   {
      pInterface->setDpoint( number, DPOINT[number].status, DPOINT[number].value );
   }
}

void CxDataProvider::sendExternalApoint( uint16_t number )
{
   // put in to external storage
   CxDataConnection *pInterface = getInterface();  
   if (pInterface != 0)
   {
      pInterface->setApoint( number, APOINT[number].status, APOINT[number].value );
   }
}

void CxDataProvider::subscribeOnExternalDpoint( uint16_t number )
{
   // put in to external storage
   CxDataConnection *pInterface = getInterface();  
   if (pInterface != 0)
   {
      pInterface->setNotification( number, DtDigitalPoint );
   }
}

void CxDataProvider::subscribeOnExternalApoint( uint16_t number )
{
   // put in to external storage
   CxDataConnection *pInterface = getInterface();  
   if (pInterface != 0)
   {
      pInterface->setNotification( number, DtAnalogPoint );
   }
}     

//--------------------------------D point data provider-------------------------

TDPOINT & CxDataProvider::getDPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);
  
   subscribeOnExternalDpoint( number );

   return DPOINT[number];
}

void CxDataProvider::setDPoint( uint16_t number, uint16_t value )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   // we should reduce server trafic
   if (value != DPOINT[number].value)
   {
      DPOINT[number].value = value;

      sendExternalDpoint( number );
   }
}

void CxDataProvider::incDPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].value++;

   sendExternalDpoint( number );
}

void CxDataProvider::decDPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].value--;

   sendExternalDpoint( number );
}
     
void CxDataProvider::setDStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   // we should reduce server trafic
   if (status != DPOINT[number].status)
   {
      DPOINT[number].status |= status;
      sendExternalDpoint( number );      
   }
}

int8_t CxDataProvider::getDStatus( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   subscribeOnExternalDpoint( number );

   return DPOINT[number].status;
}

void CxDataProvider::setSilenceDPoint( uint16_t number, uint16_t value )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].value = value;
}

void CxDataProvider::setSilenceDStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].status |= status;
}

//--------------------------------A point data provider-------------------------

TAPOINT & CxDataProvider::getAPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   subscribeOnExternalApoint( number );

   return APOINT[number];
}

void CxDataProvider::setAPoint( uint16_t number, float value )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   if (value != APOINT[number].value)
   {
      APOINT[number].value = value;
      sendExternalApoint( number );
   }
}

void CxDataProvider::setAStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   // we should reduce server trafic
   if (status != APOINT[number].status)
   {
      APOINT[number].status = status;
      sendExternalApoint( number );
   }
}

int8_t CxDataProvider::getAStatus( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   subscribeOnExternalApoint( number );

   return APOINT[number].status;
}
