#ifndef _CX_LOCAL_DBG
#define _CX_LOCAL_DBG

//------------------------------------------------------------------------------

#include "DebugConfig.h"
#include "CxDebugBase.h"

//------------------------------------------------------------------------------

class CxLocalDbg
{
 public: 
   
   CxLocalDbg( unsigned short sID );
   ~CxLocalDbg( ){}   
   
   void sendMessage( const char* pFormat, ...);//const;
   void sendError( const char* pFormat, ...) const;

 protected:   
   
   bool IsDbgActiv();           // true if scop is active
   
 private:       
   
   CxDebugBase &dbgBase;
   
   unsigned short scopID;
      
   bool ScopIsActive;           // true = active 
   
   // don't copy or default - create CHBTraceScopes
   CxLocalDbg( CxLocalDbg const & other );
   CxLocalDbg& operator=( CxLocalDbg const & other );
}; 
typedef CxLocalDbg *pTCxLocalDbg;

#endif // _CX_LOCAL_DBG 
