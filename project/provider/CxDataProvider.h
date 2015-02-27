#ifndef _CX_DATA_PROVIDER
#define _CX_DATA_PROVIDER

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "CxMutex.h"
#include "IxEventProducer.h"

//------------------------------------------------------------------------------
#define d_point_total 100
#define a_point_total 100

//---------points status----------
#define STATUS_UNKNOWN           0  // "UNKNOWN" - link with host - error
#define STATUS_RELIABLE          1  // "OK"
#define STATUS_ALARM             2  // "ALARM"
#define STATUS_BEFORE_CRASH      4  // "BEFORE CRASH"
#define STATUS_CRASH             8  // "CRASH"
#define STATUS_SETNEW            16 // set new value of point 
#define STATUS_PROCESSED         32 // processed of new value of point flag

//------------------------------------------------------------------------------

#pragma pack ( 1 )

struct TDPOINT
{
   uint8_t  number;
   int8_t   status;
   uint16_t value;
}; typedef TDPOINT *pTDPOINT; 

struct TAPOINT
{
   uint8_t number;
   int8_t  status;
   float   value;
}; typedef TAPOINT *pTAPOINT;

#pragma pack ( )

//------------------------------------------------------------------------------

class CxDataProvider: public IxEventProducer
{
   public:
      // function's
      CxDataProvider();
      ~CxDataProvider();

   protected:

      TDPOINT & getDPoint( uint8_t number );
      void setDPoint( uint8_t number, uint16_t value );
      void incDPoint( uint8_t number );
      void decDPoint( uint8_t number );
      void setDStatus( uint8_t number, int8_t status );
      int8_t getDStatus( uint8_t number );

      TAPOINT & getAPoint( uint8_t number );
      void setAPoint( uint8_t number, float value );
      void setAStatus( uint8_t number, int8_t status );
      int8_t getAStatus( uint8_t number );

      void  resetDStatus( uint8_t number, int8_t status );
      void  resetAStatus( uint8_t number, int8_t status );

   private:

      static TDPOINT DPOINT[d_point_total];
      static TAPOINT APOINT[a_point_total];
      static CxMutex digitalDataProviderMutex;
      static CxMutex analogDataProviderMutex;

 }; typedef CxDataProvider *pTCxDataProvider;

//------------------------------------------------------------------------------

#endif // _CX_DATA_PROVIDER

