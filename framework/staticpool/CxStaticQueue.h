#ifndef _CX_STATIC_QUEUE
#define _CX_STATIC_QUEUE

//------------------------------------------------------------------------------

#include "..\pal\utils.h"
#include "CxStaticPool.h"
#include "CxMutex.h"

//-------------------------------static FIFO  declaration-----------------------
//--- FIFO ---

template <class TElement>
class CxStaticQueue
{
 public:
   CxStaticQueue( char count );
   ~CxStaticQueue( ){}
   
   bool put( const TElement& );
   bool get( TElement& );
   bool isEmpty( )const{return top == 0;}
   bool isFull ( )const{return top == size-1;}
 private:
   CxMutex QueueMutex;
   unsigned short size;
   char     top;
   TElement* queuePtr;
};

template <class TElement>
CxStaticQueue<TElement>::CxStaticQueue( char count ):
   QueueMutex( )
{
  QueueMutex.take();
  
    // reset variable
    queuePtr = NULL;
      
    CxStaticPool &StaticPool = CxStaticPool::getInstance();
    // get memory
    TElement* tmpQueuePtr = reinterpret_cast<TElement*>( StaticPool.pStaticMalloc(sizeof(TElement)*count) );
    mod_memset( reinterpret_cast<char*>(tmpQueuePtr), 0, sizeof(TElement)*count, sizeof(TElement)*count );
    
    if( tmpQueuePtr != NULL )
    {
      size = count;
      top  = 0;
      queuePtr = tmpQueuePtr;
    }    
    
  QueueMutex.give();
}

template <class TElement>
bool CxStaticQueue<TElement>::put( const TElement& inElement )
{
  bool result = false;
  
  QueueMutex.take();  
    if( isFull() == false && queuePtr != NULL )
    {
      queuePtr[top++] = inElement;
      result = true;
    }  
  QueueMutex.give();   
  
  return result;
}

template <class TElement>
bool CxStaticQueue<TElement>::get( TElement& outElement )
{
  bool result = false;
  
  QueueMutex.take(); 
    if( isEmpty() == false && queuePtr != NULL )
    {
      outElement = *queuePtr;
      LShiftArray( reinterpret_cast<char*>(queuePtr),size*sizeof(TElement),sizeof(TElement) );    
      top--;
      result = true;
    }  
  QueueMutex.give();   
  
  return result;
}

#endif /*_CX_STATIC_QUEUE*/   
