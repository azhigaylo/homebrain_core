#ifndef _CX_MUTEX
#define _CX_MUTEX


//----------------------only wraper for FREERTOS xQueue ------------------------

#include "FreeRTOS.h"
#include "semphr.h"


//------------------------------------------------------------------------------


class CxMutex
{
public:
  
   CxMutex( );
   ~CxMutex( );

   void take( );
   void give( );

private:

   CxMutex( const CxMutex& rhs );
   CxMutex& operator=( const CxMutex& rhs );

protected:
     
   xSemaphoreHandle mutex; 
  
};

#endif

