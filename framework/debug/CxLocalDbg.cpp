//------------------------------------------------------------------------------

#include <stdarg.h>

#include "CxLocalDbg.h"
#include "DebugConfig.h"

//------------------------------------------------------------------------------

CxLocalDbg::CxLocalDbg( uint16_t sID ):
   pDbgBase( CxDebugBase::getInstance() )
  ,scopID( sID )   
{   
  ScopIsActive = pDbgBase->IsScopeActive( sID );  
}

//------------------------------------------------------------------------------

void CxLocalDbg::sendMessage( const char* pFormat, ...)const
{
   va_list argPtr;
   va_start(argPtr, pFormat);

   if( true == ScopIsActive )
   {    
      pDbgBase->dbgMessage( pFormat, &argPtr ); 
   }  

   va_end(argPtr);
}

void CxLocalDbg::sendError( const char* pFormat, ...)const
{
   va_list argPtr;
   va_start(argPtr, pFormat);

   pDbgBase->dbgError( pFormat, &argPtr );

   va_end(argPtr);
}

//------------------------------------------------------------------------------
