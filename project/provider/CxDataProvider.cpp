#include "..\framework\debug\DebugMacros.h"
#include "CxDataProvider.h"
#include "Utils.h"

//--------------------------------data provider class---------------------------

CxDataProvider::CxDataProvider() 
{
  // clearing discret point
  for( unsigned short point=0; point < d_point_total; point++ )
  {
    DPOINT[point].number = point;
    DPOINT[point].status = 0;
    DPOINT[point].value = 0;
  }
  // clearing analog point  
  for( unsigned short point=0; point < a_point_total; point++ )
  {
    APOINT[point].number = point;
    APOINT[point].status = 0;  
    APOINT[point].value = 0;
  }
  // clearing text point  
  for( unsigned short point=0; point < t_point_total; point++ )
  {
    TEXTPOINT[point].number = point;
    TEXTPOINT[point].status = 0;   
    mod_memset( TEXTPOINT[point].text, 0, sizeof(TEXTPOINT[0].text), sizeof(TEXTPOINT[0].text) );  
  }    
} 

CxDataProvider::~CxDataProvider(){

}

CxDataProvider &CxDataProvider::getInstance()
{
  static CxDataProvider theInstance;
  return theInstance;
}

//--------------------------------D point data provider-------------------------

TDPOINT & CxDataProvider::getDPoint( unsigned char number )
{  
  return DPOINT[number];
}

void CxDataProvider::setDPoint( unsigned char number, unsigned short value )
{
  DPOINT[number].value = value;
  sendEvent( CxEvent::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::incDPoint( unsigned char number )
{
  DPOINT[number].value++;
  sendEvent( CxEvent::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::decDPoint( unsigned char number )
{
  DPOINT[number].value--;
  sendEvent( CxEvent::EVENT_DP_NEW_VALUE, reinterpret_cast<void*>(&DPOINT[number]) );
}
     
void CxDataProvider::setDStatus( unsigned char number, char status )
{
  DPOINT[number].status |= status;
  sendEvent( CxEvent::EVENT_DP_NEW_STATUS, reinterpret_cast<void*>(&DPOINT[number]) );
}

void CxDataProvider::resetDStatus( unsigned char number, char status )
{
  char data = static_cast<char>(~(static_cast<int>(status)) );
  DPOINT[number].status &= data;
  sendEvent( CxEvent::EVENT_DP_NEW_STATUS, reinterpret_cast<void*>(&DPOINT[number]) );
}

//--------------------------------A point data provider-------------------------

TAPOINT & CxDataProvider::getAPoint( unsigned char number )
{  
  return APOINT[number];
}

void CxDataProvider::setAPoint( unsigned char number, float value )
{
  APOINT[number].value   = value;
  sendEvent( CxEvent::EVENT_AP_NEW_VALUE, reinterpret_cast<void*>(&APOINT[number]) );
}

void CxDataProvider::setAStatus( unsigned char number, char status )
{
  APOINT[number].status |= status;
  sendEvent( CxEvent::EVENT_AP_NEW_STATUS, reinterpret_cast<void*>(&APOINT[number]) );  
}

void CxDataProvider::resetAStatus( unsigned char number, char status )
{
  char data = static_cast<char>(~(static_cast<int>(status)) );
  APOINT[number].status &= data;
  sendEvent( CxEvent::EVENT_AP_NEW_STATUS, reinterpret_cast<void*>(&APOINT[number]) );  
}

//--------------------------------T point data provider-------------------------

TTEXTPOINT & CxDataProvider::getTPoint( unsigned char number )
{
  return TEXTPOINT[number];
}

void CxDataProvider::pushChar( unsigned char number, char symbol )
{
  // get text size
  char str_len = mod_strlen( TEXTPOINT[number].text, sizeof(TEXTPOINT[number].text) );
  
  switch (symbol)
  { 
    case 0x08 : {
       popChar( number );
       break;
    } 
    default   : {
       if( str_len < sizeof(TEXTPOINT[number].text) )
       {
         // print symbol in free position
         TEXTPOINT[number].text[str_len] = symbol;
       } 
       break;
    }       
  } // switch..
  
  sendEvent( CxEvent::EVENT_TP_NEW_VALUE, reinterpret_cast<void*>(&TEXTPOINT[number]) );  
}

char CxDataProvider::popChar( unsigned char number )
{
  // get text size
  char str_len = mod_strlen( TEXTPOINT[number].text, sizeof(TEXTPOINT[number].text) );
  char tmp_char = TEXTPOINT[number].text[--str_len];
  //clear char position
  TEXTPOINT[number].text[str_len] = 0;

  return tmp_char;
}

void CxDataProvider::putString( unsigned char number, char* text )
{
  // clear old content
  mod_memset( TEXTPOINT[number].text, 0, sizeof(TEXTPOINT[number].text), sizeof(TEXTPOINT[number].text) );
  // set new string
  char str_len = mod_strlen( text, sizeof(TEXTPOINT[number].text) );
  mod_memcpy( TEXTPOINT[number].text, text, str_len, sizeof(TEXTPOINT[number].text) );
  
  sendEvent( CxEvent::EVENT_TP_NEW_VALUE, reinterpret_cast<void*>(&TEXTPOINT[number]) );   
}

void CxDataProvider::setTStatus( unsigned char number, char status )
{
  TEXTPOINT[number].status |= status;
  sendEvent( CxEvent::EVENT_TP_NEW_STATUS, reinterpret_cast<void*>(&TEXTPOINT[number]) ); 
}

void CxDataProvider::resetTStatus( unsigned char number, char status )
{
  char data = static_cast<char>(~(static_cast<int>(status)) );
  TEXTPOINT[number].status &= data;
  sendEvent( CxEvent::EVENT_TP_NEW_STATUS, reinterpret_cast<void*>(&TEXTPOINT[number]) ); 
}

