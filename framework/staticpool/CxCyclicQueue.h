#ifndef _CX_CYCLIC_QUEUE
#define _CX_CYCLIC_QUEUE

#include "..\pal\utils.h"
#include "CxMutex.h"
#include "CxStaticPool.h"

//---------------cyclic queue definition ---------------------------------------

template <class TElement>
class CxCyclicQueue
{
 public:
   
   CxCyclicQueue( char count );
   ~CxCyclicQueue(){}
   
   bool put( const TElement& );
   bool get( TElement& );

 private:      

   unsigned short numberOfElements;

   TElement* pQueueStartElement;
   TElement* pQueueLastElement;
   
   TElement* pElementForGet;
   TElement* pElementForPut;   
   

   CxCyclicQueue( CxCyclicQueue const & other );
   CxCyclicQueue& operator=( CxCyclicQueue const & other );
};

//------------------------------------------------------------------------------

template <class TElement>
CxCyclicQueue<TElement>::CxCyclicQueue( char count ):
   numberOfElements   ( 0    )
  ,pQueueStartElement ( NULL )      
  ,pQueueLastElement  ( NULL )  
  ,pElementForGet     ( NULL )  
  ,pElementForPut     ( NULL )     
{ 
   CxStaticPool &StaticPool = CxStaticPool::getInstance();
   // get memory
   TElement* tmpQueuePtr = reinterpret_cast<TElement*>( StaticPool.pStaticMalloc(sizeof(TElement)*count) );
   
   mod_memset( reinterpret_cast<char*>(tmpQueuePtr), 0, sizeof(TElement)*count, sizeof(TElement)*count );
  
   if( tmpQueuePtr != NULL )
   {
     pElementForGet = pElementForPut = pQueueStartElement = pQueueLastElement = tmpQueuePtr;
     
     pQueueLastElement = pQueueStartElement + (count-1);
   } 
   else
   {
     // !!!
   }  
}

//------------------------------------------------------------------------------

template <class TElement>
bool CxCyclicQueue<TElement>::put( const TElement& inElement )
{
  bool result = false;
        
  if( pQueueStartElement != NULL )
  {  
    if( (pElementForPut == pElementForGet) && (numberOfElements > 0) )
    {
      // queue is full - skip request
    }
    else
    {
      // queue not full - OK     

      // save element      
      *pElementForPut = inElement;      
      // inc pointer
      if( pElementForPut == pQueueLastElement )
      {
        pElementForPut = pQueueStartElement;
      }
      else
      {
        pElementForPut++;
      } 
      // inc number of elements
      numberOfElements++;
      
      result = true;
    }  
  }  

  return result;
}

template <class TElement>
bool CxCyclicQueue<TElement>::get( TElement& outElement )
{
  bool result = false;
        
  if( pQueueStartElement != NULL )
  {  
    if( (pElementForPut == pElementForGet) && (numberOfElements == 0) )
    {
      // queue is empty - nothing to do
    }
    else
    {
      // queue not empty - OK  
      
      // out element
      outElement = *pElementForGet;
      // inc pointer
      if( pElementForGet == pQueueLastElement )
      {
        pElementForGet = pQueueStartElement;
      }
      else
      {
        pElementForGet++;
      } 
      // inc number of elements
      numberOfElements--;
      
      result = true;      
    }
  }  
  return result;
}

#endif /*_CX_CYCLIC_QUEUE*/   

