#ifndef _CX_MUTEXRECURSIVE
#define _CX_MUTEXRECURSIVE


//----------------------only wraper for FREERTOS xQueue ------------------------

#include "FreeRTOS.h"
#include "semphr.h"

//------------------------------------------------------------------------------


class CxMutexRecursive
{
public:
  
   CxMutexRecursive();
   ~CxMutexRecursive();

   void take();
   void give();

private:

   CxMutexRecursive(const CxMutexRecursive& rhs);
   CxMutexRecursive& operator=(const CxMutexRecursive& rhs);

protected:
     
   xSemaphoreHandle mutex; 
  
};

#endif

