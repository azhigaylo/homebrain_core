#ifndef _CXDEBUG_PROCESSOR
#define _CXDEBUG_PROCESSOR

//------------------------------------------------------------------------------ 
#include <stdarg.h>
#include "DebugConfig.h"
#include "IxRunnable.h"
#include "CxMutex.h"
#include "PSysI.h"
#include "..\devctrl\CxLogDevice.h"
#include "..\staticpool\CxCyclicQueue.h"
#include "..\eventpool\IxEventConsumer.h"

//------------------------------------------------------------------------------ 

#define configCONTEINER_SIZE MAX_TRACE_SIZE                                     // in byte
#define configDBGBLOCK_SIZE  ( 50 )                                             // in items

//------------------------------------------------------------------------------ 
#pragma pack ( 1 )
struct TDBGMSG
{
  unsigned char Size;
  unsigned char DbgString[configCONTEINER_SIZE];
}; 
#pragma pack ( )
typedef TDBGMSG *pTDBGMSG; 

//------------------------------------------------------------------------------ 

class CxDebugProcessor : public IxRunnable, public CxLogDevice, public IxEventConsumer
{
 public: 
   
    static CxDebugProcessor& getInstance( );    

    bool PutDbgMsgInQueu( const char* pFormat, va_list *dataList );
    
    virtual void  Start();
       
 private:   

    CxDebugProcessor();
    ~CxDebugProcessor( ){}
            
    // internal container DBGMSG protector 
    CxMutex queueMutex;
    // cyclic queue with messages
    CxCyclicQueue<TDBGMSG>CyclicQueue;    
    // current message    
    TDBGMSG DBGMSG;
      
    // task processor
    virtual void TaskProcessor( );                                              // from IxRunnable
    virtual bool processEvent( pTEvent pEvent );   
    
    bool transmitterAreFree;
    
}; 
typedef CxDebugProcessor *pCxDebugProcessor;

#endif /*_CXDEBUG_PROCESSOR*/   
