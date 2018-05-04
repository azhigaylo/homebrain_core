//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutexLocker.h"
#include "staticpool/CxStaticPool.h"
//------------------------------------------------------------------------------
//-------------------------------STATIC POOL class------------------------------
//------------------------------------------------------------------------------
CxMutex CxStaticPool::StaticPoolMutex("pool_mutex");
CxStaticPool *CxStaticPool::theInstance = 0;
//------------------------------------------------------------------------------

CxStaticPool::CxStaticPool():
    sRecNumb( 0 )
   ,pHead   ( staticpool )
{
   memset( staticpool, 0, configSTATICPOOL_SIZE );
}

CxStaticPool *CxStaticPool::getInstance()
{
   if(CxStaticPool::theInstance == 0)
   {
      CxMutexLocker locker(&CxStaticPool::StaticPoolMutex);

      if(CxStaticPool::theInstance == 0)
      {
         CxStaticPool::theInstance = new CxStaticPool();
      }
   }

   return CxStaticPool::theInstance;
}

void CxStaticPool::delInstance()
{
   if(CxStaticPool::theInstance != 0)
   {
      delete CxStaticPool::theInstance;
   }
}

void* CxStaticPool::pStaticMalloc( unsigned short xSize )
{
  void *pTmpHead = NULL;

  CxMutexLocker locker(&CxStaticPool::StaticPoolMutex);

  if( pHead != NULL && ( GetFreeSize() >= xSize) )
  {
    pTmpHead = pHead;
    pHead += xSize;
    sRecNumb++;

    printDebug("CxStaticPool/%s: allocated size = %d  ", __FUNCTION__, xSize);
  }
  else
  {
    pTmpHead = pHead;

    printError("CxStaticPool/%s: bad allocation with size = %d  ", __FUNCTION__, xSize);
  }

  return pTmpHead;
}

unsigned short CxStaticPool::GetFreeSize() const
{
   return(unsigned short)((staticpool + configSTATICPOOL_SIZE) - pHead );
}
