//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "IxDriver.h"
//------------------------------------------------------------------------------

IxDriver::IxDriver( const char *pcName ):
   DrvID         ( CRC16_T(const_cast<char*>(pcName), strlen_m(const_cast<char*>(pcName), configMAX_DRIVER_NAME_LEN)) )  
  ,ConsumerID    ( 0 )  
  ,taskdID       ( 0 )  
  ,startTime     ( time(NULL) )  
  ,inQueue       ( strcat(strncpy_m( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) ), "_in"), 10, sizeof(TCommand) )
  ,outQueue      ( strcat(strncpy_m( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) ), "_out"), 10, sizeof(TCommand) )
  ,initAttempt   ( 0 )  
{
   strncpy_m( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) );  
} 

//------------------------------------------------------------------------------

IxDriver::~IxDriver()
{
   task_delete();
}

//------------------------------------------------------------------------------

void IxDriver::task_delete( )
{  
   if (taskdID != 0)
   {
      pthread_cancel(taskdID);
   
      printDebug("IxDriver/%s: driver=%s deleted", __FUNCTION__, pcDrvName);
   }
}   

//------------------------------------------------------------------------------

void IxDriver::task_run( )
{
   create_thread(); 
}
  
//------------------------------------------------------------------------------

int32_t IxDriver::create_thread( )
{
   int32_t task_result = 0;

   task_result = pthread_create(&taskdID, NULL, thRunnableFunction, this);

   if (task_result != 0) 
   {
      printError("IxDriver/%s: driver=%s error!!!", __FUNCTION__, pcDrvName);
   }
   else
   {
      pthread_setname_np(taskdID, pcDrvName);
   }

   return task_result;
}

//------------------------------------------------------------------------------

uint64_t IxDriver::get_time()
{
   return difftime(time(NULL), startTime); 
}
  
//------------------------------------------------------------------------------

void * IxDriver::thRunnableFunction( void *args )
{   
   (reinterpret_cast<IxDriver*>(args))->run();
}

void IxDriver::run()
{
  while(true)
  {   
    DrvProcessor();
  }  
} 

//------------------------------------------------------------------------------

void IxDriver::DrvProcessor()
{
   TCommand Command = { 0, 0, 0, 0, NULL };

   if (-1 != inQueue.receive(reinterpret_cast<void*>(&Command), sizeof(TCommand)))
   {
      if (Command.ConsumerID == DrvID)
      {
         // command is mine
         if ((Command.ComType == identification_request) && (Command.ComID == DIReq))
         {
            printDebug("IxDriver/%s: ConsumerID=%d, SenderID=%d,ComType=%d, ComID=%d ", __FUNCTION__, Command.ConsumerID, Command.SenderID, Command.ComType, Command.ComID);

            // remember current consumer
            ConsumerID = Command.SenderID;

            // set up resonce for top level driver
            Command.ConsumerID = Command.SenderID;  
            Command.SenderID   = DrvID;               
            Command.ComType    = identification_response;
            Command.ComID      = DIRes;

            outQueue.send( reinterpret_cast<const void*>(&Command), sizeof(TCommand) );   

            initAttempt++;
         }
         else
         {
            CommandProcessor( Command );
         }  
      }        
   }  

   // if driver was initialised - call thread processor
   if (initAttempt > 0)
   {  
      ThreadProcessor( ); 
   }
}

/*
uint16_t counter_item = 0;

void IxDriver::ThreadProcessor( )
{
   counter_item++;
   
   TCommand Command = { 0, 0, 0, 0, NULL };
   // set up resonce for top level driver
   Command.ConsumerID = 10;  
   Command.SenderID   = 11;               
   Command.ComType    = 12;
   Command.ComID      = counter_item;

   outQueue.send(reinterpret_cast<const void*>(&Command), sizeof(TCommand)); 

   sleep_s(5);
}
*/