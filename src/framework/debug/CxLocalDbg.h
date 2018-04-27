#ifndef _CX_LOCAL_DBG
#define _CX_LOCAL_DBG

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "DebugConfig.h"
#include "CxDebugBase.h"
//------------------------------------------------------------------------------

class CxLocalDbg
{
   public:

      CxLocalDbg( uint16_t sID );
      ~CxLocalDbg( ){}

      void sendMessage( const char* pFormat, ...) const;
      void sendError( const char* pFormat, ...) const;

   protected:

      bool IsDbgActiv();           // true if scop is active

   private:

      CxDebugBase *pDbgBase;

      uint16_t scopID;

      bool ScopIsActive;           // true = active

      // don't copy or default - create CHBTraceScopes
      CxLocalDbg( CxLocalDbg const & other );
      CxLocalDbg& operator=( CxLocalDbg const & other );
};
typedef CxLocalDbg *pTCxLocalDbg;

#endif // _CX_LOCAL_DBG
