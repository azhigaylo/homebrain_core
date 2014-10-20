//------------------------------------------------------------------------------

#include <stdarg.h>

#include "CxLocalDbg.h"
#include "DebugConfig.h"

//------------------------------------------------------------------------------

CxLocalDbg::CxLocalDbg( unsigned short sID ):
   dbgBase( CxDebugBase::getInstance() )
  ,scopID( sID )   
{   
  ScopIsActive = dbgBase.IsScopeActive( sID );  
}

//------------------------------------------------------------------------------

void CxLocalDbg::sendMessage( const char* pFormat, ...) // const
{
  va_list argPtr;
  va_start(argPtr, pFormat);

  if( true == ScopIsActive )
  {    
    dbgBase.dbgMessage( pFormat, &argPtr );    
  }  
  
  va_end(argPtr);     
}

void CxLocalDbg::sendError( const char* pFormat, ...)const
{
    va_list argPtr;
    va_start(argPtr, pFormat);
         
    dbgBase.dbgError( pFormat, &argPtr );
    
    va_end(argPtr);  
}

//------------------------------------------------------------------------------
