/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_DATA_PROVIDER
#define _CX_DATA_PROVIDER

//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "os_wrapper/CxMutex.h"
#include "eventpool/IxEventProducer.h"
#include "interface/CxInterface.h"
//------------------------------------------------------------------------------
#define d_point_total 1000
#define a_point_total 1000

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
       uint16_t number;
       int8_t   status;
       uint16_t value;
    }; typedef TDPOINT *pTDPOINT;

    struct TAPOINT
    {
       uint16_t number;
       int8_t   status;
       double   value;
    }; typedef TAPOINT *pTAPOINT;

#pragma pack ( )

//------------------------------------------------------------------------------

class CxDataProvider: public IxEventProducer
{
     // function's
     CxDataProvider();
     ~CxDataProvider();

   public:
      static CxDataProvider &getInstance();

      TDPOINT& getDPoint    ( uint16_t number );
      int8_t   getDStatus   ( uint16_t number );
      void setDPoint        ( uint16_t number, uint16_t value );
      void setDStatus       ( uint16_t number, int8_t status );
      void setSilenceDPoint ( uint16_t number, uint16_t value );
      void setSilenceDStatus( uint16_t number, int8_t status );

      TAPOINT& getAPoint    ( uint16_t number );
      int8_t   getAStatus   ( uint16_t number );
      void setAPoint        ( uint16_t number, double value );
      void setAStatus       ( uint16_t number, int8_t status );
      void setSilenceAPoint ( uint16_t number, double value );
      void setSilenceAStatus( uint16_t number, int8_t status );
   private:

      TAPOINT APOINT[a_point_total];
      TDPOINT DPOINT[d_point_total];

      static CxMutex digitalDataProviderMutex;
      static CxMutex analogDataProviderMutex;

 }; typedef CxDataProvider *pTCxDataProvider;

//------------------------------------------------------------------------------

#endif // _CX_DATA_PROVIDER

