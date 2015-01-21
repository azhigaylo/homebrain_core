#ifndef _CXDEBUG_PROCESSOR
#define _CXDEBUG_PROCESSOR

//------------------------------------------------------------------------------ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "ptypes.h"
#include "IxRunnable.h"
#include "DebugConfig.h"
#include "CxCyclicQueue.h"
//------------------------------------------------------------------------------

#define configCONTEINER_SIZE MAX_TRACE_SIZE                 // in byte
#define configDBGBLOCK_SIZE  ( 50 )                         // in items

//------------------------------------------------------------------------------
#pragma pack ( 1 )
struct TDBGMSG
{
   uint8_t Size;
   char DbgString[configCONTEINER_SIZE];
}; 
#pragma pack ( )
typedef TDBGMSG *pTDBGMSG; 

//------------------------------------------------------------------------------

class CxDebugProcessor : public IxRunnable
{
   public:
      CxDebugProcessor();
      ~CxDebugProcessor( ){}

      bool PutDbgMsgInQueu( const char* pFormat, va_list *dataList );

      virtual void  Start();

   private:

      // internal container DBGMSG protector 
      static CxMutex queueMutex;
      // cyclic queue with messages
      CxCyclicQueue<TDBGMSG>CyclicQueue;    
      // current message    
      TDBGMSG DBGMSG;

      // task processor
      virtual void TaskProcessor( );                        // from IxRunnable

      bool transmitterAreFree;
}; 
typedef CxDebugProcessor *pCxDebugProcessor;

#endif /*_CXDEBUG_PROCESSOR*/
