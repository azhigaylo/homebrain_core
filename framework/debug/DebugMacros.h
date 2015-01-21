#ifndef DEBUG_MACROS
#define DEBUG_MACROS

//------------------------------------------------------------------------------
#include "utils.h"
#include "CxDebugBase.h"
#include "CxLocalDbg.h"

//------------------------------------------------------------------------------

#define MKSTRING( str ) #str

//------------------------------------------------------------------------------

// scope declaretion
#define  DBG_SCOPE_DEF(className, methodName) \
  {const char* SCOPE_NAME = MKSTRING(className##_##methodName); \
    CxDebugBase::getInstance()->ScopeRegistration( SCOPE_NAME );} 

// scope definition 
#define  DBG_SCOPE(className, methodName) \
  const char* SCOPE_NAME = MKSTRING(className##_##methodName); \
   CxLocalDbg localScope( CRC16_T( const_cast<char*>(SCOPE_NAME), strlen_m(SCOPE_NAME, MAX_SCOPE_NAME_LENGTH) ) );

// debug message generation
#define  DBG_MSG(args) \
   do {\
      localScope.sendMessage args ; \
   } while(false)

#define  DBG_ERR(args) \
   do {\
      localScope.sendError args ; \
   } while(false)

#endif // DEBUG_CONFIG
