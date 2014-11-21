#include "CxQueue.h"

//------------------------------------------------------------------------------

CxQueue::CxQueue( unsigned long queueLength, unsigned long itemSize )
{
   xQueue = NULL;
   Create( queueLength, itemSize );
}

CxQueue::~CxQueue()
{
   Delete( );
}

//------------------------------------------------------------------------------

bool CxQueue::Send( void *pItemToQueue )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    result = static_cast<bool>(xQueueSend( xQueue, pItemToQueue, ( portTickType ) 0 ));
  }  
  return result;  
}

bool CxQueue::SendToBack( void *pItemToQueue )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    result = static_cast<bool>(xQueueSendToBack( xQueue, pItemToQueue, ( portTickType ) 0 ));
  }  
  return result;  
}

bool CxQueue::SendToFront( void *pItemToQueue )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    result = static_cast<bool>(xQueueSendToFront( xQueue, pItemToQueue, ( portTickType ) 0 ));
  }  
  return result;  
}

bool CxQueue::Receive( void *pItemFromQueue )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    result = static_cast<bool>(xQueueReceive( xQueue, pItemFromQueue, ( portTickType ) 0 ));
  }  
  return result;  
}

bool CxQueue::Peek( void *pItemFromQueue )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    result = static_cast<bool>(xQueuePeek( xQueue, pItemFromQueue, ( portTickType ) 0 ));
  }  
  return result;  
}

bool CxQueue::AddToRegistry( signed char *Name )
{
  bool result = false;
  if( xQueue != NULL ) 
  {  
    vQueueAddToRegistry( xQueue, Name );
    result = true;
  }  
  return result;    
}

unsigned long CxQueue::Occupancy( )
{
  unsigned long result = 0;
  if( xQueue != NULL ) 
  {  
    result = static_cast<unsigned long>(uxQueueMessagesWaiting( xQueue ));
  }  
  return result;    
}

void CxQueue::Create( unsigned long queueLength, unsigned long itemSize )
{
   xQueue = xQueueCreate( queueLength, itemSize );
}

void CxQueue::Delete( )
{
  vQueueDelete( xQueue );
}
