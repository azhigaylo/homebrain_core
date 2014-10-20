#ifndef _CX_DATA_PROVIDER
#define _CX_DATA_PROVIDER

#include "..\eventpool\IxEventProducer.h"

//------------------------------------------------------------------------------
#define d_point_total 10
#define a_point_total 10 
#define t_point_total 2
//------------------------------------------------------------------------------

//---------points status----------
#define STATUS_UNKNOWN            0x00,      // "UNKNOWN" - linck with device - error
#define STATUS_RELIABLE           0x01,      // "OK"
#define STATUS_ALARM              0x02,      // "ALARM"
#define STATUS_BEFORE_CRASH       0x04,      // "BEFORE CRASH"
#define STATUS_CRASH              0x08       // "CRASH"

#pragma pack ( 1 )

struct TDPOINT
{
  char number;
  char status;
  unsigned short value;
}; typedef TDPOINT *pTDPOINT; 

struct TAPOINT
{
  char number;
  char status;
  float value;
}; typedef TAPOINT *pTAPOINT;

struct TTEXTPOINT
{    
  char number;
  char status;
  char text[11];
}; typedef TTEXTPOINT *pTTEXTPOINT; 

#pragma pack ( )

//------------------------------------------------------------------------------

class CxDataProvider: public IxEventProducer 
{
  public:                     

     static CxDataProvider& getInstance();
     
     TDPOINT & getDPoint( unsigned char number );
     void setDPoint( unsigned char number, unsigned short value );
     void incDPoint( unsigned char number );
     void decDPoint( unsigned char number );
     void setDStatus( unsigned char number, char status );
     
     TAPOINT & getAPoint( unsigned char number );
     void setAPoint( unsigned char number, float value );
     void setAStatus( unsigned char number, char status );

     
     TTEXTPOINT & getTPoint( unsigned char number );
     void pushChar( unsigned char number, char symbol );
     char popChar( unsigned char number );
     void putString( unsigned char number, char* text );
     void setTStatus( unsigned char number, char status );

     
  protected:    

     void resetDStatus( unsigned char number, char status );    
     void resetAStatus( unsigned char number, char status );
     void resetTStatus( unsigned char number, char status );
     
     // function's   
     CxDataProvider();
     ~CxDataProvider();
     
  private:    
    
    TDPOINT DPOINT[d_point_total];
    TAPOINT APOINT[a_point_total];
    TTEXTPOINT TEXTPOINT[t_point_total];
     
 }; typedef CxDataProvider *pTCxDataProvider;
 
//------------------------------------------------------------------------------

#endif // _CX_DATA_PROVIDER

