#include "CxStaticPool.h"
#include "CxAssertion.h"
//------------------------------------------------------------------------------
//-------------------------------STATIC POOL class------------------------------
//------------------------------------------------------------------------------

CxStaticPool::CxStaticPool()
{   
   sRecNumb = 0;    
   mod_memset( staticpool, 0, configSTATICPOOL_SIZE, configSTATICPOOL_SIZE );
   pHead = staticpool;
}

CxStaticPool &CxStaticPool::getInstance()
{
  static CxStaticPool theInstance;
  return theInstance;
}

unsigned short CxStaticPool::GetFreeSize()const
{
  return( (staticpool + configSTATICPOOL_SIZE) - pHead );
}

void* CxStaticPool::pStaticMalloc( size_t xSize )
{
  void *pTmpHead = NULL;
  
  StaticPoolMutex.take();
  
  if( pHead != NULL && ( GetFreeSize() >= xSize) )
  {
    pTmpHead = pHead;
    pHead += xSize;
    sRecNumb++;    
  }
  else 
  {
    pTmpHead = pHead;
    vApplicationStaticPoolFailedHook();
  }
  
  StaticPoolMutex.give();
  
  return reinterpret_cast<void*>(pTmpHead);
}
