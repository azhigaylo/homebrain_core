#ifndef _CX_LIST
#define _CX_LIST

#include "CxStaticPool.h"

//-------------------------------static stack  declaration----------------------
#pragma pack ( 1 )
template <class TElem>
struct TAlgIntBlkList
{
  TAlgIntBlkList* pNextIntBlk;     // next block pointer 
  TAlgIntBlkList* pPrevIntBlk;     // previous block pointer 
  TElem     element;         // element
};
#pragma pack ( )


template <class TElement>
class CxList
{
 public:
   CxList(char count);
   virtual ~CxList(){}
     
   TElement & operator[]( unsigned short pos );
   bool add             ( const TElement & element );
   bool clear           ( );
   bool remove          ( TElement* pElement );
   bool insert          ( unsigned short pos, const TElement & element );
   TElement &first      ( );
   TElement &last       ( );
   short indexOf        ( TElement* pElement );
   unsigned short count ( ){ return size;}
   
 private:      

   typedef TAlgIntBlkList<TElement> TIntBlk;
   
   TIntBlk* GetIntBlkByElement(TElement* pElement);

   unsigned short size;
   TIntBlk* pFirstIntBlk;     // 
   TIntBlk* pLastIntBlk;      // 
   
   CxStaticPoolAllocator<TIntBlk> CONTAINER;       // container for editor object  

};

template <class TElement>
CxList<TElement>::CxList(char count):
                                    CONTAINER     (count),
                                    pFirstIntBlk  (NULL),
                                    pLastIntBlk   (NULL),
                                    size          (0)
{

}

template <class TElement>
bool CxList<TElement>::add( const TElement & element )
{
  bool result = false;
    
  TIntBlk* pIntBlk = CONTAINER.getContainer( );
  
  if( pIntBlk != NULL )
  {
    if( size == 0 )
    {
       pIntBlk->pNextIntBlk = NULL;
       pIntBlk->pPrevIntBlk = NULL;  
       pFirstIntBlk = pLastIntBlk = pIntBlk;      
    }
    else{
        pLastIntBlk->pNextIntBlk = pIntBlk;         
        pIntBlk->pPrevIntBlk     = pLastIntBlk;  
        pLastIntBlk = pIntBlk;        
    }    
    
    pIntBlk->element = element;
    
    size++;    
    
    result = true;
  }

  return result;   
}

template <class TElement>
bool CxList<TElement>::clear( )
{
  bool result = true;
   
  while( (size != 0) && (pLastIntBlk != NULL) )
  {
    
    TIntBlk* pIntBlk = pLastIntBlk; 
    pLastIntBlk = pIntBlk->pPrevIntBlk;
    if( CONTAINER.delContainer( pIntBlk ) == false )
    {
      result = false;
      break;
    }

    if( --size == 0)
    {
      pFirstIntBlk = pLastIntBlk = NULL; 
    }  
    
  }

  return result;
}

template <class TElement>
bool CxList<TElement>::remove( TElement* pElement )
{
  bool result = false;
    
  TIntBlk* pIntBlk = GetIntBlkByElement( pElement );
  
  if( (pIntBlk != NULL) && (indexOf(pElement) != -1) )
  {    
    // case - deleting of fist or single object
    if( pIntBlk == pFirstIntBlk )
    {      
      if( pIntBlk->pNextIntBlk != NULL)
      {
        pIntBlk->pNextIntBlk->pPrevIntBlk = NULL;
        pFirstIntBlk = pIntBlk->pNextIntBlk;
      }
      else
      {
        pFirstIntBlk = pLastIntBlk = NULL;
      }  
      
      result = CONTAINER.delContainer( pIntBlk );  

    }
    else
    {
      // case - deleting of last object
      if( pIntBlk == pLastIntBlk )
      {
  
        pIntBlk->pPrevIntBlk->pNextIntBlk = NULL;
        pLastIntBlk = pIntBlk->pPrevIntBlk;
          
        result = CONTAINER.delContainer( pIntBlk );  
        
      }
      else
      {
        // case - deleting of middle object
        pIntBlk->pPrevIntBlk->pNextIntBlk = pIntBlk->pNextIntBlk;
        pIntBlk->pNextIntBlk->pPrevIntBlk = pIntBlk->pPrevIntBlk;
          
        result = CONTAINER.delContainer( pIntBlk ); 
      }        
    }

    size--;
            
  } 
  
  return result;
}

template <class TElement>
TElement & CxList<TElement>::operator[]( unsigned short pos )                   // interval 1 - 65536
{
  TElement* pElement = NULL;  
  unsigned  short pointer = 0;
  TIntBlk*  pIntBlk = pFirstIntBlk;

  while( pIntBlk != NULL )
  {     
    if( pointer == pos )
    {
      pElement = &(pIntBlk->element); 
      break;
    }
    // get next block
    pIntBlk = pIntBlk->pNextIntBlk;    
    pointer++;    
  }

  return *pElement;
}

template <class TElement>
bool CxList<TElement>::insert( unsigned short pos, const TElement & element )   // insert element in position = pos
{
  bool result = false;
     
  // create new element
  TIntBlk* pNewIntBlk = CONTAINER.getContainer( );
  
  // search element with position pos 
  unsigned  short pointer = 0;
  TIntBlk*  pIntBlk = pFirstIntBlk; 

  while( pIntBlk != NULL )
  {     
    if( pointer == pos ){ break; }
    // get next block
    pIntBlk = pIntBlk->pNextIntBlk;    
    pointer++;    
  }
        
  // insert new element in sequence
  if( pNewIntBlk != NULL )
  {            
    if( (size != 0) && (pIntBlk != NULL) )
    {
       // case - deleting of fist or single object
       if( pIntBlk == pFirstIntBlk )
       {
          
          pNewIntBlk->pNextIntBlk = pIntBlk;
          pNewIntBlk->pPrevIntBlk = NULL;
          pIntBlk->pPrevIntBlk = pNewIntBlk;
          pFirstIntBlk = pNewIntBlk;        
          
       }
       else
       {
         // case - deleting of last object
         if( pIntBlk == pLastIntBlk )
         {      
            pNewIntBlk->pNextIntBlk = NULL;
            pNewIntBlk->pPrevIntBlk = pIntBlk;          
            pIntBlk->pNextIntBlk = pNewIntBlk;
            pLastIntBlk = pNewIntBlk;                 
         }
         else
         {         
            pNewIntBlk->pNextIntBlk = pIntBlk;
            pNewIntBlk->pPrevIntBlk = pIntBlk->pPrevIntBlk;  
            
            pIntBlk->pPrevIntBlk->pNextIntBlk = pNewIntBlk;
            pIntBlk->pPrevIntBlk = pNewIntBlk;                
         }   
       }
    }
    else
    {
      if(size == 0)
      { 
         pNewIntBlk->pNextIntBlk = NULL;
         pNewIntBlk->pPrevIntBlk = NULL;  
         pFirstIntBlk = pLastIntBlk = pNewIntBlk;  
      }
      else
      {
        CONTAINER.delContainer( pNewIntBlk );
        return false;
      }  
    }   
    
    pNewIntBlk->element = element;
    
    size++;        
    result = true;
  }
        
  return result;
}

template <class TElement>
TElement & CxList<TElement>::first()
{
  TElement* result = NULL;
  
  if( pFirstIntBlk != NULL )
  {    
    result = &(pFirstIntBlk->element); 
  }
  
  return *result;
}

template <class TElement>
TElement & CxList<TElement>::last()
{
  TElement* result = NULL;
  
  if( pLastIntBlk != NULL )
  {
    
    result = &(pLastIntBlk->element);  
    
  }
  
  return *result;
}

template <class TElement>
short CxList<TElement>::indexOf( TElement* pElement )
{
  short position = -1;
  TIntBlk*  pIntBlk = pFirstIntBlk;

  while( pIntBlk != NULL )
  {                  
    position++;    
    if( &(pIntBlk->element) == pElement ) return position;    
    pIntBlk = pIntBlk->pNextIntBlk;
  }

  return -1;
}

template <class TElement>
CxList<TElement>::TIntBlk* CxList<TElement>::GetIntBlkByElement(TElement* pElement)
{
  TIntBlk* pIntBlk = NULL;
  
  if( pElement != NULL )
  {
    
    pIntBlk = reinterpret_cast<TIntBlk*>( reinterpret_cast<char*>(pElement) - (sizeof(pIntBlk->pNextIntBlk) + sizeof(pIntBlk->pPrevIntBlk)) );
 
  }  
  
  return pIntBlk;    
}

#endif /*_CX_STATIC_POOL*/   

