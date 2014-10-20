#ifndef _CX_DEBUG_BASE
#define _CX_DEBUG_BASE

//------------------------------------------------------------------------------
#include "utils.h"
#include "..\staticpool\CxList.h"
#include "DebugConfig.h"
#include "CxDebugProcessor.h"

//------------------------------------------------------------------------------
#pragma pack ( 1 )
struct TScopeListItem
{
  unsigned short number;
  unsigned short hashCode;
}; 
#pragma pack ( )
typedef TScopeListItem *pTScopeListItem; 
//------------------------------------------------------------------------------


class CxDebugBase
{
 public: 
   
   static CxDebugBase& getInstance();

   void ScopeRegistration( const char* pScopeName );
   bool IsScopeActive( unsigned short sID );
   
   void dbgMessage( const char* pFormat, va_list *dataList );
   void dbgError  ( const char* pFormat, va_list *dataList );   
   
 protected:   
   
   CxDebugBase();
   ~CxDebugBase( ){}

 private:       

   CxList<TScopeListItem> SCOPE_LIST;           // scope in processing  
   
   CxDebugProcessor &debugProcessor;
}; 
typedef CxDebugBase *pTCxDebugBase;

#endif // _CX_DEBUG_BASE 
