#ifndef _CX_DEBUG_BASE
#define _CX_DEBUG_BASE

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "utils.h"
#include "CxList.h"
#include "DebugConfig.h"
#include "CxDebugProcessor.h"
//------------------------------------------------------------------------------
#pragma pack ( 1 )
struct TScopeListItem
{
  uint16_t number;
  uint16_t hashCode;
}; 
#pragma pack ( )
typedef TScopeListItem *pTScopeListItem; 
//------------------------------------------------------------------------------

class CxDebugBase
{
 public:

   static CxDebugBase * getInstance();
   void delInstance();

   void ScopeRegistration( const char* pScopeName );
   bool IsScopeActive( uint16_t sID );

   void dbgMessage( const char* pFormat, va_list *dataList );
   void dbgError  ( const char* pFormat, va_list *dataList );

 protected:
   
   CxDebugBase();
   ~CxDebugBase( ){}

 private:

   CxList<TScopeListItem> SCOPE_LIST;           // scope in processing
   CxDebugProcessor debugProcessor;

   static CxMutex singlDebugLock;
   static CxDebugBase* theInstance;
};
typedef CxDebugBase *pTCxDebugBase;

#endif // _CX_DEBUG_BASE 
