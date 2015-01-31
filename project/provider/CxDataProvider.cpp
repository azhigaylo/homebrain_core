//------------------------------------------------------------------------------
#include "utils.h"
#include "DebugMacros.h"
#include "CxMutexLocker.h"
#include "CxDataProvider.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//--------------------------------data provider class---------------------------
CxMutex CxDataProvider::digitalDataProviderMutex("d_data_mutex");
CxMutex CxDataProvider::analogDataProviderMutex("a_data_mutex");
TDPOINT CxDataProvider::DPOINT[d_point_total];
TAPOINT CxDataProvider::APOINT[a_point_total];
//------------------------------------------------------------------------------

CxDataProvider::CxDataProvider() 
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

//--------------------------------D point data provider-------------------------

TDPOINT & CxDataProvider::getDPoint( uint8_t number )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  return DPOINT[number];
}

void CxDataProvider::setDPoint( uint8_t number, uint16_t value )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  DPOINT[number].value = value;
  sendEvent( event_pool::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::incDPoint( uint8_t number )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  DPOINT[number].value++;
  sendEvent( event_pool::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::decDPoint( uint8_t number )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  DPOINT[number].value--;
  sendEvent( event_pool::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}
     
void CxDataProvider::setDStatus( uint8_t number, int8_t status )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  DPOINT[number].status |= status;
  sendEvent( event_pool::EVENT_DP_NEW_STATUS, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::resetDStatus( uint8_t number, int8_t status )
{
  CxMutexLocker locker(&CxDataProvider::digitalDataProviderMutex);

  DPOINT[number].status &= ~status;
  sendEvent( event_pool::EVENT_DP_NEW_STATUS, reinterpret_cast<void*>(&DPOINT[number]) );
}

//--------------------------------A point data provider-------------------------

TAPOINT & CxDataProvider::getAPoint( uint8_t number )
{
  CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);
  
  return APOINT[number];
}

void CxDataProvider::setAPoint( uint8_t number, float value )
{
  CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

  APOINT[number].value   = value;
  sendEvent( event_pool::EVENT_AP_NEW_VALUE, reinterpret_cast<void*>(&APOINT[number]) );
}

void CxDataProvider::setAStatus( uint8_t number, int8_t status )
{
  CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

  APOINT[number].status |= status;
  sendEvent( event_pool::EVENT_AP_NEW_STATUS, reinterpret_cast<void*>(&APOINT[number]) );
}

void CxDataProvider::resetAStatus( uint8_t number, int8_t status )
{
  CxMutexLocker locker(&CxDataProvider::analogDataProviderMutex);

  APOINT[number].status &= ~status;
  sendEvent( event_pool::EVENT_AP_NEW_STATUS, reinterpret_cast<void*>(&APOINT[number]) );
}
