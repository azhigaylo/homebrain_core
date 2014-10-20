#ifndef _CX_STATIC_POOL
#define _CX_STATIC_POOL

#include "..\pal\utils.h"
#include "CxMutex.h"

#define configSTATICPOOL_SIZE ( 6000 )     // in byte

//-------------------------------Static pool declaration------------------------

class CxStaticPool
{
 public: 
    static CxStaticPool& getInstance();
    
    void *pStaticMalloc( size_t xSize );
    unsigned short GetFreeSize( )const;
    virtual ~CxStaticPool( ){}    

 protected:   
    CxStaticPool();

 private:              
    unsigned short sRecNumb;
    char* pHead;    
    char staticpool[configSTATICPOOL_SIZE];
    
    CxMutex StaticPoolMutex;
    
}; 
typedef CxStaticPool *pTCxStaticPool;


//-------------------------------Static pool allocator declaration--------------
#pragma pack ( 1 )
struct TMemBlockHeader
{
   unsigned short cElemCount;
   unsigned short sBlkSize;
   TMemBlockHeader* pNextMemBlockHeader;
};
#pragma pack ( )

typedef TMemBlockHeader* pTMemBlockHeader;

template <class TElement>
class CxStaticPoolAllocator
{
 public:
   CxStaticPoolAllocator( unsigned short count );
   ~CxStaticPoolAllocator( ){}
   
   bool isEmpty( )const;
   TElement* getContainer( );
   TElement* getContainer( const TElement& );   
   bool delContainer( TElement* );
   bool isContainerBusy( TElement* )const;    // if conteyner is Busy - function terurn "true"
   unsigned short getSize( )const;
   TElement* searchByFunktion( bool(*execution_func)(TElement*) );
   
 private:
   
   CxMutex SPAmutex;
   
   struct TIntBlk
   {
     bool busy;     // if true - busy
     TElement element;
   };
   
   // Block - memory field in CxStaticPool
   // element - data part - TElement
   pTMemBlockHeader pStrtBlock;   
   pTMemBlockHeader pCurentBlock;      
   unsigned short sElementsCount;
   
   TElement* getFreeElementPosition( );  
   pTMemBlockHeader createOneMoreBlock( );
   pTMemBlockHeader getLastBlock( );
   pTMemBlockHeader getNextBlock( );   
};

//--------------------CxStaticPoolAllocator--------------
template <class TElement>
CxStaticPoolAllocator<TElement>::CxStaticPoolAllocator( unsigned short count ):
   SPAmutex( )
{
   SPAmutex.take();    
   
      CxStaticPool &StaticPool = CxStaticPool::getInstance();
     
      pTMemBlockHeader pTmpStrtBlock = reinterpret_cast<pTMemBlockHeader>( StaticPool.pStaticMalloc(sizeof(TMemBlockHeader) + sizeof(TIntBlk)*count) );
      if( pTmpStrtBlock )
      {
        pStrtBlock = pCurentBlock = pTmpStrtBlock;     
        mod_memset( reinterpret_cast<char*>(pStrtBlock), 0, sizeof(TMemBlockHeader) + sizeof(TIntBlk)*count, sizeof(TMemBlockHeader) + sizeof(TIntBlk)*count );
        pStrtBlock->sBlkSize = sizeof(TIntBlk)*count;
        pStrtBlock->pNextMemBlockHeader = NULL;
        pStrtBlock->cElemCount = 0;
        sElementsCount = 0;       
      }
      
   SPAmutex.give();
}

template <class TElement>
bool CxStaticPoolAllocator<TElement>::isEmpty( )const
{
  if( sElementsCount != 0 ) return true;
    else return false;
}

template <class TElement>
TElement* CxStaticPoolAllocator<TElement>::getContainer( const TElement& element )
{  
   SPAmutex.take();
   
    TElement* pNewElement = getFreeElementPosition();
    if( pNewElement != NULL )
    {
      *pNewElement = element;
      sElementsCount++;
    }
    else
    {
      if( createOneMoreBlock() != NULL )
      {
         pNewElement = getFreeElementPosition();
         if( pNewElement != NULL )
         {
           *pNewElement = element;
           sElementsCount++;
         }
      }  
    }      
    
  SPAmutex.give();    
  return pNewElement;
}

template <class TElement>
TElement* CxStaticPoolAllocator<TElement>::getContainer()
{  
  SPAmutex.take();
  
    TElement* pNewElement = getFreeElementPosition();
    if(pNewElement != NULL)
    {
      sElementsCount++;
    }
    else
    {
      if( createOneMoreBlock() != NULL )
      {
         pNewElement = getFreeElementPosition();
         if( pNewElement != NULL )
         {
           sElementsCount++;
         }
      }  
    }      
    
  SPAmutex.give();  
  return pNewElement;
}

template <class TElement>
bool CxStaticPoolAllocator<TElement>::delContainer( TElement* pElement )
{
  SPAmutex.take();
  
    pTMemBlockHeader pMemBlockHeader = pStrtBlock;
    if(pMemBlockHeader != NULL)
    {
      while(pMemBlockHeader != NULL)
      {      
        if(pMemBlockHeader->cElemCount)
        {        
          // free space in this block is to be available 
          TIntBlk* pIntBlk = reinterpret_cast<TIntBlk*>(reinterpret_cast<char*>(pMemBlockHeader) + sizeof(TMemBlockHeader));
          
          for( unsigned short iterator = 0; iterator <= static_cast<unsigned short>(pMemBlockHeader->sBlkSize/sizeof(TIntBlk)); iterator++, pIntBlk++ )
          {            
            if( (&(pIntBlk->element) == pElement) && (pIntBlk->busy == true) )
            {
              mod_memset( reinterpret_cast<char*>(pIntBlk), 0, sizeof(TIntBlk), sizeof(TIntBlk) );            
              pMemBlockHeader->cElemCount--;
              sElementsCount--;
              SPAmutex.give();               
              return true;
            }
          }  
          
        }                  
        pMemBlockHeader = pMemBlockHeader->pNextMemBlockHeader;  
      }  
    }      
    
  SPAmutex.give();  
  return false;
}

template <class TElement>
bool CxStaticPoolAllocator<TElement>::isContainerBusy( TElement* pElement )const
{
  for( unsigned short i=0; i<sizeof(TElement); i++ )
  {
    if( *(reinterpret_cast<char*>(pElement) + i ) != 0 ) return true;    
  }  
  return false;
}

template <class TElement>
unsigned short CxStaticPoolAllocator<TElement>::getSize()const
{
  return sElementsCount;
}

template <class TElement>
TElement* CxStaticPoolAllocator<TElement>::searchByFunktion( bool (*execution_func)(TElement* pElement) )
{
  pTMemBlockHeader pMemBlockHeader = pStrtBlock;
  if( pMemBlockHeader != NULL )
  {
     while( pMemBlockHeader != NULL )
     {      
       if( pMemBlockHeader->cElemCount )
       {        
         // free space in this block is to be available 
         TIntBlk* pIntBlk = reinterpret_cast<TIntBlk*>(reinterpret_cast<char*>(pMemBlockHeader) + sizeof(TMemBlockHeader));
         for( unsigned short iterator = 0; iterator < pMemBlockHeader->sBlkSize/static_cast<unsigned short>(sizeof(TIntBlk)); iterator++, pIntBlk++ )
         {
           if( pIntBlk->busy == true )
           {
             if( true == (*execution_func)(&(pIntBlk->element)) )
             {
               return( &(pIntBlk->element) ); 
             }
           }           
         }  
       }                  
       pMemBlockHeader = pMemBlockHeader->pNextMemBlockHeader;  
     }  
  }      
  return NULL;
}

template <class TElement>
TElement* CxStaticPoolAllocator<TElement>::getFreeElementPosition()
{
  pTMemBlockHeader pMemBlockHeader = pStrtBlock;
  TElement* pFirstFreePos = NULL;
    
  if( pMemBlockHeader != NULL )
  {  
    while( pMemBlockHeader != NULL )
    {
      if( pMemBlockHeader->cElemCount*sizeof(TIntBlk) < pMemBlockHeader->sBlkSize )
      {        
        // free space in this block is to be available 
        TIntBlk* pIntBlk = reinterpret_cast<TIntBlk*>(reinterpret_cast<char*>(pMemBlockHeader) + sizeof(TMemBlockHeader));
               
        for( unsigned short iterator = 0; iterator <= static_cast<unsigned short>(pMemBlockHeader->sBlkSize/sizeof(TIntBlk)); iterator++, pIntBlk++ )
        {                     
          if( pIntBlk->busy == false )
          {
            pIntBlk->busy = true;
            pMemBlockHeader->cElemCount++;
            pFirstFreePos = &(pIntBlk->element);
            break;
          }
        }        
        break;
      }        
      pMemBlockHeader = pMemBlockHeader->pNextMemBlockHeader;  
    }
  }   
  
  return( pFirstFreePos );  
}

template <class TElement>
pTMemBlockHeader CxStaticPoolAllocator<TElement>::getNextBlock()
{
  if( pCurentBlock->pNextMemBlockHeader != NULL ) pCurentBlock = pCurentBlock->pNextMemBlockHeader; 
  return( pCurentBlock->pNextMemBlockHeader );  
}

template <class TElement>
pTMemBlockHeader CxStaticPoolAllocator<TElement>::getLastBlock()
{
  pTMemBlockHeader pMemBlockHeader = pStrtBlock;
  if( pMemBlockHeader != NULL )
  {
    while( pMemBlockHeader->pNextMemBlockHeader != NULL )
    {
      pMemBlockHeader = pMemBlockHeader->pNextMemBlockHeader;  
    }  
  }  
  return pMemBlockHeader;  
}

template <class TElement>
pTMemBlockHeader CxStaticPoolAllocator<TElement>::createOneMoreBlock()
{
   CxStaticPool &StaticPool = CxStaticPool::getInstance();
   pTMemBlockHeader pLastMemBlockHeader = getLastBlock();
   
   pTMemBlockHeader pTmpBlock = reinterpret_cast<pTMemBlockHeader>( StaticPool.pStaticMalloc(sizeof(TMemBlockHeader) 
                                                              +   sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1)) );
   if( pTmpBlock != NULL && pLastMemBlockHeader != NULL )
   {
     pLastMemBlockHeader->pNextMemBlockHeader = pTmpBlock;     
     mod_memset(reinterpret_cast<char*>(pTmpBlock), 0, sizeof(TMemBlockHeader) + sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1),
                                                     sizeof(TMemBlockHeader)  + sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1));
     pTmpBlock->sBlkSize = sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1);
     pTmpBlock->pNextMemBlockHeader = NULL;
     pTmpBlock->cElemCount = 0;
     sElementsCount = 0;       
   }
   
  return pTmpBlock;
}

#endif /*_CX_STATIC_POOL*/   
