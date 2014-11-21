//------------------------------------------------------------------------------

#include "CxThreadIO.h"
#include "Utils.h"

//------------------------------------------------------------------------------

CxThreadIO::CxThreadIO(  portCHAR * taskName,  portCHAR * drvName ):
    IxRunnable    ( taskName )
  , CxSystemQueue ( )
  , threadIOState ( ST_IO_UNKNOWN )
  , drvName       ( drvName )
  , drvID         ( CRC16_T(drvName, mod_strlen(drvName, configMAX_TASK_NAME_LEN))  )
  , threadID      ( CRC16_T(taskName, mod_strlen(taskName, configMAX_TASK_NAME_LEN)) )  
{

} 

CxThreadIO::~CxThreadIO( )
{

}

void CxThreadIO::Start()
{ 
  // create thread
  task_run( );

}

//------------------------------------------------------------------------------

void CxThreadIO::TaskProcessor()
{
  
  switch( threadIOState )
  {    
      case ST_IO_UNKNOWN : 
      {         
        threadIOState = ST_IO_DRV_REQUEST;                                            // put in next state
        break;
      }
      case ST_IO_DRV_REQUEST :  
      { 
        DriverIdentificationRequest();
        threadIOState = ST_IO_WAIT_DRV_RESP;                                     // put in next state
        break;
      }
      case ST_IO_WAIT_DRV_RESP :  
      { 
        if( true == CheckDrvCommand() ) 
        {
           threadIOState = ST_IO_NORMAL_WORK;                                   // put in next state        
        }  
        break;
      }	
      case ST_IO_NORMAL_WORK :  
      {         
        CheckDrvCommand();
        ThreadProcessor();
        break;
      }	      

      default : break;
   }
  
   

}

//------------------------------------------------------------------------------

void CxThreadIO::DriverIdentificationRequest()
{
    TCommand Command = { threadID, drvID, identification_request, DIReq, NULL };
    SendCommand( &Command );        
}

bool CxThreadIO::CheckDrvCommand()
{
  bool result = false;
  
  TCommand Command = { 0, 0, 0, 0, NULL };

  if( IsCommand() > 0 )
  {
    if( true == PeekCommand( &Command ) )
    {
      if( Command.ConsumerID == threadID && Command.SenderID == drvID )
      {
        if( true == ReceiveCommand ( &Command ) )      
        {
          // command is mine
          if( (Command.ComType == identification_response) && (Command.ComID == DIRes) )
          {
             result = true;
          } 
          else
          {
             CommandProcessor( Command );
          }           
        }        
      }  
    }
  }
  
  return result;
}
   
//------------------------------------------------------------------------------
