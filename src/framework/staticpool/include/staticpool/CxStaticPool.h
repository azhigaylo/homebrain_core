#ifndef _CX_STATIC_POOL
#define _CX_STATIC_POOL

//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutex.h"
//------------------------------------------------------------------------------
#define configSTATICPOOL_SIZE ( 20480 )     // in byte
//------------------------------------------------------------------------------


//-------------------------------Static pool declaration------------------------

class CxStaticPool
{
   public:

      static CxStaticPool *getInstance();
      void delInstance();

      void *pStaticMalloc( unsigned short xSize );

   private:

      CxStaticPool();
      ~CxStaticPool( ){}
      unsigned short GetFreeSize( ) const;

      unsigned short sRecNumb;
      char* pHead;
      static CxMutex StaticPoolMutex;
      static CxStaticPool* theInstance;

      char staticpool[configSTATICPOOL_SIZE];
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
   SPAmutex("SPAmutex")
{
   SPAmutex.lock();

      CxStaticPool *pStaticPool = CxStaticPool::getInstance();

      pTMemBlockHeader pTmpStrtBlock = reinterpret_cast<pTMemBlockHeader>( pStaticPool->pStaticMalloc((unsigned short)(sizeof(TMemBlockHeader) + sizeof(TIntBlk)*count)));
      if( pTmpStrtBlock )
      {
        pStrtBlock = pCurentBlock = pTmpStrtBlock;
        memset( reinterpret_cast<char*>(pStrtBlock), 0, sizeof(TMemBlockHeader) + sizeof(TIntBlk)*count);
        pStrtBlock->sBlkSize = (unsigned short)(sizeof(TIntBlk)*count);
        pStrtBlock->pNextMemBlockHeader = NULL;
        pStrtBlock->cElemCount = 0;
        sElementsCount = 0;
      }

   SPAmutex.unlock();
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
   SPAmutex.lock();

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

  SPAmutex.unlock();

  return pNewElement;
}

template <class TElement>
TElement* CxStaticPoolAllocator<TElement>::getContainer()
{
  SPAmutex.lock();

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

  SPAmutex.unlock();

  return pNewElement;
}

template <class TElement>
bool CxStaticPoolAllocator<TElement>::delContainer( TElement* pElement )
{
  SPAmutex.lock();

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
              memset( reinterpret_cast<char*>(pIntBlk), 0, sizeof(TIntBlk));
              pMemBlockHeader->cElemCount--;
              sElementsCount--;
              SPAmutex.unlock();
              return true;
            }
          }

        }
        pMemBlockHeader = pMemBlockHeader->pNextMemBlockHeader;
      }
    }

  SPAmutex.unlock();
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
   CxStaticPool *pStaticPool = CxStaticPool::getInstance();
   pTMemBlockHeader pLastMemBlockHeader = getLastBlock();

   pTMemBlockHeader pTmpBlock = reinterpret_cast<pTMemBlockHeader>(
                                pStaticPool->pStaticMalloc((unsigned short)(sizeof(TMemBlockHeader) + sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1))));
   if( pTmpBlock != NULL && pLastMemBlockHeader != NULL )
   {
     pLastMemBlockHeader->pNextMemBlockHeader = pTmpBlock;
     memset(reinterpret_cast<char*>(pTmpBlock), 0, sizeof(TMemBlockHeader) + sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1));
     pTmpBlock->sBlkSize = (unsigned short)(sizeof(TIntBlk)*(pStrtBlock->cElemCount/2 + 1));
     pTmpBlock->pNextMemBlockHeader = NULL;
     pTmpBlock->cElemCount = 0;
     sElementsCount = 0;
   }

  return pTmpBlock;
}

#endif /*_CX_STATIC_POOL*/
