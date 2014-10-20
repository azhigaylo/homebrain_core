//------------------------------------------------------------------------------

#include "CxDebugBase.h"

//------------------------------------------------------------------------------

CxDebugBase::CxDebugBase( ):  
   SCOPE_LIST( MAX_TRACE_PACKAGE )
  ,debugProcessor( CxDebugProcessor::getInstance() )  
{   

}

CxDebugBase &CxDebugBase::getInstance( )
{
  static CxDebugBase theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxDebugBase::ScopeRegistration( const char* pScopeName )
{
  TScopeListItem scopeListItem;
  scopeListItem.number   =  SCOPE_LIST.count();
  scopeListItem.hashCode = CRC16_T( pScopeName, mod_strlen(pScopeName, MAX_SCOPE_NAME_LENGTH) );
  
  
  if( true != SCOPE_LIST.add( scopeListItem ) )
  {
    // vax vax plexo :(
  }      
}

//------------------------------------------------------------------------------

bool CxDebugBase::IsScopeActive( unsigned short sID )
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
