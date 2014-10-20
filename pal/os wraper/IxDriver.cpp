//------------------------------------------------------------------------------

#include "CxDriverManager.h"
#include "IxDriver.h"
#include "Utils.h"

//------------------------------------------------------------------------------

IxDriver::IxDriver( portCHAR * pcName ):   
   CxSystemQueue ( ) 
  ,DrvID         ( CRC16_T(pcName, mod_strlen(pcName, configMAX_TASK_NAME_LEN)) )  
  ,ConsumerID    ( 0 )  
  ,taskdID       ( 0 )    
  ,initAttempt   ( 0 )  
{
   mod_memset( pcDrvName, 0, sizeof(pcDrvName), sizeof(pcDrvName) );  
   mod_strncpy( pcDrvName, (char*)pcName, sizeof(pcDrvName) );
   
   // registration in drv manager   
   registration();
} 

//------------------------------------------------------------------------------

IxDriver::~IxDriver()
{

}

//------------------------------------------------------------------------------

void IxDriver::task_suspend( )
{ 
   vTaskSuspend( taskdID );
}      

//------------------------------------------------------------------------------

void IxDriver::task_delete( )
{
   vTaskDelete( taskdID );
}   

//------------------------------------------------------------------------------

void IxDriver::task_sleep( portTickType xTicksToDelay )
{
   vTaskDelay( xTicksToDelay );  
}

//------------------------------------------------------------------------------

void IxDriver::task_run( )
{
   create_thread(); 
}

//------------------------------------------------------------------------------

portBASE_TYPE IxDriver::create_thread( )
{
   signed portBASE_TYPE task_result = 0;

   task_result = xTaskCreate( thRunnableFunction, (const signed portCHAR *)pcDrvName,  configMINIMAL_STACK_SIZE, this, tskIDLE_PRIORITY + 1, &taskdID );

   return task_result;
}

//------------------------------------------------------------------------------

unsigned long IxDriver::get_sys_tick()
{
  return  xTaskGetTickCount( );
}
  
//------------------------------------------------------------------------------

void IxDriver::thRunnableFunction( void *pvParameters )
{   
  (reinterpret_cast<IxDriver*>(pvParameters))->RUN();
}

void IxDriver::RUN()
{
  for( ;; )
  {   
    DrvProcessor();
    vTaskDelay( 5 );      
  }  
} 

//------------------------------------------------------------------------------

void IxDriver::registration()
{
  CxDriverManager &driverManager = CxDriverManager::getInstance();  
  driverManager.driverRegistration ( DrvID, this );
}

void IxDriver::DrvProcessor()
{
  TCommand Command = { 0, 0, 0, 0, NULL };

  if( IsCommand() > 0 )
  {
    if( true == PeekCommand( &Command ) )
    {
      if( Command.ConsumerID == DrvID )
      {
        if( true == ReceiveCommand ( &Command ) )      
        {
          // command is mine
          if( (Command.ComType == identification_request) && (Command.ComID == DIReq) )
          {
             // remember current consumer
             ConsumerID = Command.SenderID;
             // set up resonce for top level driver
             Command.ConsumerID = Command.SenderID;  
             Command.SenderID   = DrvID;               
             Command.ComType    = identification_response;
             Command.ComID      = DIRes;
             SendCommand( &Command );
            
             initAttempt++;
          }
          else
          {
             CommandProcessor( Command );
          }  
        }        
      }  
    }
  }
  
  // if driver was initialised - call thread processor
  if( initAttempt > 0 )
  {  
    ThreadProcessor( ); 
  }
}
