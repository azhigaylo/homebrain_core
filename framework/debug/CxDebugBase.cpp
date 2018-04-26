//------------------------------------------------------------------------------
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutexLocker.h"
#include "CxDebugBase.h"
//------------------------------------------------------------------------------
CxDebugBase* CxDebugBase::theInstance = 0;
CxMutex CxDebugBase::singlDebugLock("singlDebugLocker");
//------------------------------------------------------------------------------
CxDebugBase::CxDebugBase( ):  
    SCOPE_LIST( MAX_TRACE_PACKAGE )
   ,debugProcessor( )
{
   debugProcessor.Start();
}

CxDebugBase *CxDebugBase::getInstance( )
{
   if(theInstance == 0)
   {
      CxMutexLocker locker(&CxDebugBase::singlDebugLock);

      if(theInstance == 0)
      {
         theInstance = new CxDebugBase;
      }
   }

  return theInstance;
}

void CxDebugBase::delInstance()
{
   if(CxDebugBase::theInstance != 0)
   {
       // remove singleton item
       delete this;
   }
}

//------------------------------------------------------------------------------

void CxDebugBase::ScopeRegistration( const char* pScopeName )
{
  TScopeListItem scopeListItem;
  scopeListItem.number   =  SCOPE_LIST.count();
  scopeListItem.hashCode = CRC16_T( const_cast<char*>(pScopeName), strlen_m(pScopeName, MAX_SCOPE_NAME_LENGTH) );

  if( true != SCOPE_LIST.add( scopeListItem ) )
  {
     // vax vax plexo :(
     printWarning("CxDebugBase/%s:scope %s was not registerd", __FUNCTION__, pScopeName);
  }
}

//------------------------------------------------------------------------------

bool CxDebugBase::IsScopeActive( uint16_t sID )
{
  bool result = false;
  for( int i=0; i < SCOPE_LIST.count(); i++)
  {
    TScopeListItem scopeListItem = SCOPE_LIST[i];
    if( sID == scopeListItem.hashCode )
    {
      result = true;
    }
  }
  return result;
}

//------------------------------------------------------------------------------

void CxDebugBase::dbgMessage( const char* pFormat, va_list *dataList )
{ 
   debugProcessor.PutDbgMsgInQueu( pFormat, dataList );
}

void CxDebugBase::dbgError( const char* pFormat, va_list *dataList )
{  
   debugProcessor.PutDbgMsgInQueu( pFormat, dataList );
}
