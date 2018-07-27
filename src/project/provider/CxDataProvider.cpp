//------------------------------------------------------------------------------
#include "common/utils.h"
#include "os_wrapper/CxMutexLocker.h"
#include "provider/CxDataProvider.h"
#include "interface/CxInterfaceManager.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//--------------------------------data provider class---------------------------
CxMutex CxDataProvider::digitalDataProviderMutex("d_data_mutex");
CxMutex CxDataProvider::analogDataProviderMutex("a_data_mutex");
//------------------------------------------------------------------------------

CxDataProvider::CxDataProvider()
{
   // clearing discret point
   for( uint16_t point=0; point < d_point_total; point++ )
   {
      DPOINT[point].number = point;
      DPOINT[point].status = STATUS_UNKNOWN;
      DPOINT[point].value = 0;
   }
   // clearing analog point
   for( uint16_t point=0; point < a_point_total; point++ )
   {
      APOINT[point].number = point;
      APOINT[point].status = STATUS_UNKNOWN;
      APOINT[point].value = 0;
   }
}

CxDataProvider::~CxDataProvider()
{

}

//------------------------------------------------------------------------------

CxDataProvider &CxDataProvider::getInstance()
{
   static CxDataProvider theInstance;
   return theInstance;
}

//--------------------------------D point data provider-------------------------

TDPOINT & CxDataProvider::getDPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   return DPOINT[number];
}

int8_t CxDataProvider::getDStatus( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   return DPOINT[number].status;
}

void CxDataProvider::setDPoint( uint16_t number, uint16_t value )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   // we should reduce server trafic
   if (value != DPOINT[number].value)
   {
      DPOINT[number].value = value;
      sendEvent( event_pool::eEventType::EVENT_DP_NEW_VALUE, number, 0);
   }
}

void CxDataProvider::setDStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   // we should reduce server trafic
   if (status != DPOINT[number].status)
   {
      DPOINT[number].status = status;
      sendEvent( event_pool::eEventType::EVENT_DP_NEW_STATUS, number, 0);
   }
}

void CxDataProvider::setSilenceDPoint( uint16_t number, uint16_t value )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].value = value;
}

void CxDataProvider::setSilenceDStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

   DPOINT[number].status = status;
}

//--------------------------------A point data provider-------------------------

TAPOINT & CxDataProvider::getAPoint( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   return APOINT[number];
}

int8_t CxDataProvider::getAStatus( uint16_t number )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   return APOINT[number].status;
}

void CxDataProvider::setAPoint( uint16_t number, double value )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   if (value != APOINT[number].value)
   {
      APOINT[number].value = value;
      sendEvent( event_pool::eEventType::EVENT_AP_NEW_VALUE, number, 0);
   }
}

void CxDataProvider::setAStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   // we should reduce server trafic
   if (status != APOINT[number].status)
   {
      APOINT[number].status = status;
      sendEvent( event_pool::eEventType::EVENT_AP_NEW_STATUS, number, 0);
   }
}

void CxDataProvider::setSilenceAPoint( uint16_t number, double value )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   APOINT[number].value = value;
}

void CxDataProvider::setSilenceAStatus( uint16_t number, int8_t status )
{
   CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

   APOINT[number].status = status;
}

