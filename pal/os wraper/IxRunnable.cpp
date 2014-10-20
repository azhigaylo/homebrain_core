#include "IxRunnable.h"
#include "Utils.h"

//--------------------------------class ----------------------------------------
IxRunnable::IxRunnable( portCHAR * pcName ):   
  taskdID( 0 )
{
   mod_memset( pcTaskName, 0, sizeof(pcTaskName), sizeof(pcTaskName) );  
   mod_strncpy( pcTaskName, (char*)pcName, sizeof(pcTaskName) );  
} 

IxRunnable::~IxRunnable()
{

}
    
void IxRunnable::task_suspend( )
{ 
   vTaskSuspend( taskdID );
}      

void IxRunnable::task_delete( )
{
   vTaskDelete( taskdID );
}   

void IxRunnable::task_sleep( portTickType xTicksToDelay )
{
   vTaskDelay( xTicksToDelay );  
}

void IxRunnable::task_run( )
{
   create_thread(); 
}

unsigned long IxRunnable::get_sys_tick()
{
  return  xTaskGetTickCount( );
}
  
portBASE_TYPE IxRunnable::create_thread( )
{
   signed portBASE_TYPE task_result = 0;

   task_result = xTaskCreate( thRunnableFunction, (const signed portCHAR *)pcTaskName,  configMINIMAL_STACK_SIZE, this, tskIDLE_PRIORITY + 1, &taskdID );

   return task_result;
}


void IxRunnable::thRunnableFunction( void *pvParameters )
{   
  (reinterpret_cast<IxRunnable*>(pvParameters))->RUN();
}

void IxRunnable::RUN()
{
  for( ;; )
  {   
    TaskProcessor();
    vTaskDelay( 5 );  
  }  
} 
