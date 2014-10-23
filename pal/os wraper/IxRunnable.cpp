#include <iostream>
using namespace std;

#include "IxRunnable.h"

//--------------------------------class ----------------------------------------

IxRunnable::IxRunnable( char *pcName ):   
  thread( 0 )
{
   strcpy_s( pcTaskName, configMAX_TASK_NAME_LEN, pcName );  
} 

IxRunnable::~IxRunnable()
{
   //task_delete( )
}
    
void IxRunnable::task_suspend( )
{ 

}      

void IxRunnable::task_delete( )
{
   //pthread_cancel(thread);
}   

void IxRunnable::task_run( )
{
   //create_thread(); 
}

unsigned long IxRunnable::get_sys_tick()
{
   return  0; 
}
  
portBASE_TYPE IxRunnable::create_thread( )
{
   int task_result = 0;

   //result = pthread_create(&thread, NULL, thRunnableFunction, this);
   if (result != 0) 
   {
      perror("thread error !");
   }
   else
   {
      //pthread_setname_np(thread, pcTaskName);
   }
	
   return task_result;
}

void IxRunnable::thRunnableFunction( void *pvParameters )
{   
  (reinterpret_cast<IxRunnable*>(pvParameters))->RUN();
}

void IxRunnable::RUN()
{
  for( int i=0; i<5; i++;)
  {   
    TaskProcessor();
    sleep( 100 );  
  }  
} 

void IxRunnable::TaskProcessor()
{
	printf("Thread %s works\n", pcTaskName);
}