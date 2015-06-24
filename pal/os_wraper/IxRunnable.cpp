//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <errno.h>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "IxRunnable.h"

//------------------------------------------------------------------------------
using namespace std;
//--------------------------------class ----------------------------------------

IxRunnable::IxRunnable( const char *pcName ):
    thread( 0 )
   ,start_time(time(NULL))
{
   strncpy_m( pcTaskName, const_cast<char*>(pcName), configMAX_TASK_NAME_LEN );
}

IxRunnable::~IxRunnable( )
{
   task_delete();
}

void IxRunnable::task_run( )
{
   create_thread();
}

void IxRunnable::task_join( )
{
   if (thread != 0)
   {
      pthread_join(thread, NULL);
   }
}

void IxRunnable::task_delete( )
{  
   if (thread != 0)
   {
      pthread_cancel(thread);
      
      pthread_join(thread, NULL);

      printDebug("IxRunnable/%s: thread=%s deleted", __FUNCTION__, pcTaskName);
   }
}

uint64_t IxRunnable::get_time()
{
   return difftime(time(NULL), start_time); 
}

int32_t IxRunnable::create_thread( )
{
   int32_t task_result = 0;

   pthread_attr_t attr;
   pthread_attr_init(&attr);
   int s = pthread_attr_setstacksize(&attr, 1048576);
   if (s != 0)
   {
      printError("IxRunnable/%s: thread=%s pthread_attr_setstacksize error!!!", __FUNCTION__, pcTaskName);
   }

   task_result = pthread_create(&thread, &attr, thRunnableFunction, this);

   if (task_result != 0)
   {
      printError("IxRunnable/%s: thread=%s error!!!", __FUNCTION__, pcTaskName);
   }
   else
   {
      pthread_setname_np(thread, pcTaskName);
   }

   return task_result;
}

void * IxRunnable::thRunnableFunction( void *args )
{
   (reinterpret_cast<IxRunnable*>(args))->run();
}

void IxRunnable::run()
{
  while(true)
  {
    TaskProcessor();
  }
}

void IxRunnable::TaskProcessor()
{
   thread = 0;
   printError("IxRunnable/%s: thread=%s not implemented, EXIT!!!", __FUNCTION__, pcTaskName);
   pthread_exit(0);
}
