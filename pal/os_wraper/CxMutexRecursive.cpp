#include "CxMutexRecursive.h"

//------------------------------------------------------------------------------
//-------------------DO NOT USE FROM ISR !!!!! ---------------------------------
//------------------------------------------------------------------------------

CxMutexRecursive::CxMutexRecursive( ):
  mutex( xSemaphoreCreateRecursiveMutex( ) )
{  
  xSemaphoreGiveRecursive( mutex );
} 

CxMutexRecursive::~CxMutexRecursive()
{
  xSemaphoreGiveRecursive( mutex );
}

void CxMutexRecursive::take()
{
  xSemaphoreTakeRecursive( mutex, portMAX_DELAY ); 
}


void CxMutexRecursive::give()
{
  xSemaphoreGiveRecursive( mutex );
}

