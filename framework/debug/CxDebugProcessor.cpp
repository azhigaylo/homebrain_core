#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "CxDebugProcessor.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------
CxDebugProcessor &debugProcessor = CxDebugProcessor::getInstance();
//------------------------------------------------------------------------------

CxDebugProcessor::CxDebugProcessor():
   IxRunnable     ( "DBG_TASK" )
  ,CxLogDevice    ( "DEV_DBG"  )   
  ,IxEventConsumer( ) 
  ,queueMutex     ( )  
  ,CyclicQueue    ( configDBGBLOCK_SIZE ) 
  ,transmitterAreFree ( true )    
    
{    
}

CxDebugProcessor &CxDebugProcessor::getInstance()
{
  static CxDebugProcessor theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

bool CxDebugProcessor::PutDbgMsgInQueu( const char* pFormat, va_list *dataList )
{
  bool result = false;
  
  queueMutex.take( );
         
    mod_memset( DBGMSG.DbgString, 0, configCONTEINER_SIZE, configCONTEINER_SIZE );
    DBGMSG.Size = mod_sprintf_new( DBGMSG.DbgString, configCONTEINER_SIZE, pFormat, dataList );
      
    if( DBGMSG.Size > 0 ) 
    {  
      result = CyclicQueue.put( DBGMSG );
    }  
    
  queueMutex.give( );

  return result;
}

//------------------------------------------------------------------------------

bool CxDebugProcessor::processEvent( pTEvent pEvent )
{
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  
  if( pEvent->eventType == intsrnalECB.TxEvent )
  {
     transmitterAreFree = true;
     return true;
   }   
   return false;    
}

//------------------------------------------------------------------------------

void CxDebugProcessor::TaskProcessor()
{
  if( (enable_operation == true) && (transmitterAreFree == true) )
  {      
    if( PORT != -1 )
    {
      queueMutex.take( );      
      
      mod_memset( DBGMSG.DbgString, 0, configCONTEINER_SIZE, configCONTEINER_SIZE );
      
      if ( true == CyclicQueue.get( DBGMSG ) )
      {
        if( -1 == FileWrite( PORT, DBGMSG.DbgString, DBGMSG.Size ) )
        {
          // reset port 
          if( -1 != FileClose( PORT ) ) PORT = -1;                        
        }
        else        
        {
          if( DBGMSG.Size > 0 ) transmitterAreFree = false;
        }  
      }
      
      queueMutex.give( );        
    }  
    else
    {
      //If port has been lost or not opened - try to reconnect one         
      if( PORT == -1 )
      {
         SwitchOn();   
      }    
    }           
  }
}

//------------------------------------------------------------------------------

void CxDebugProcessor::Start()
{
  DBG_SCOPE( CxDebugProcessor, CxDebugProcessor )

  // set notification on Tx finished event
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  setNotification( intsrnalECB.TxEvent );
  
  //
  DBG_MSG( ("[M] DBG task has been runned \n\r") );  
  // start task
  task_run( );  
}
