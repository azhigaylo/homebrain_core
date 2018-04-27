//------------------------------------------------------------------------------
#include <stdarg.h>
#include <inttypes.h>
#include <syslog.h>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <string.h>
//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
#include "DebugMacros.h"
#include "CxMutexLocker.h"
#include "CxDebugProcessor.h"
//------------------------------------------------------------------------------
CxMutex CxDebugProcessor::queueMutex("debugQueueMutex");
//------------------------------------------------------------------------------

CxDebugProcessor::CxDebugProcessor():
    IxRunnable         ( "DBG_TASK" )
   ,CyclicQueue        ( configDBGBLOCK_SIZE )
   ,transmitterAreFree ( true )
{

}

//------------------------------------------------------------------------------

bool CxDebugProcessor::PutDbgMsgInQueu( const char* pFormat, va_list *dataList )
{
   bool result = false;
   CxMutexLocker locker(&CxDebugProcessor::queueMutex);

   memset_m( DBGMSG.DbgString, 0, configCONTEINER_SIZE, configCONTEINER_SIZE );
   DBGMSG.Size = vsprintf( DBGMSG.DbgString, pFormat, *dataList );

   if( DBGMSG.Size > 0 )
   {
      result = CyclicQueue.put( DBGMSG );
   }

   return result;
}

//------------------------------------------------------------------------------

void CxDebugProcessor::TaskProcessor()
{
   CxMutexLocker locker(&CxDebugProcessor::queueMutex);
   memset_m( DBGMSG.DbgString, 0, configCONTEINER_SIZE, configCONTEINER_SIZE );

   if ( true == CyclicQueue.get( DBGMSG ) )
   {
      setlogmask (LOG_UPTO (LOG_NOTICE));
	   syslog(LOG_NOTICE, "[M] - %s", DBGMSG.DbgString);
   }

   sleep_mcs(100000);
}

//------------------------------------------------------------------------------

void CxDebugProcessor::Start()
{
  // start task
  task_run( );
}
