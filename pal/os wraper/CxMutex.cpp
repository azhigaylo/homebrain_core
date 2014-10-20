#include "CxMutex.h"

//------------------------------------------------------------------------------
//-------------------DO NOT USE FROM ISR !!!!! ---------------------------------
//------------------------------------------------------------------------------

CxMutex::CxMutex( ):
  mutex( xSemaphoreCreateMutex( ) )
{  
  xSemaphoreGive( mutex );
} 

CxMutex::~CxMutex( )
{
  xSemaphoreGive( mutex );
}

void CxMutex::take( )
{
  xSemaphoreTake( mutex, portMAX_DELAY ); 
}


void CxMutex::give( )
{
  xSemaphoreGive( mutex );
}

