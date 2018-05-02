//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/IxDriver.h"
//------------------------------------------------------------------------------

IxDriver::IxDriver( const char *pcName ):
   DrvID         ( CRC16_T(const_cast<char*>(pcName), static_cast<uint16_t>(strnlen(const_cast<char*>(pcName), configMAX_DRIVER_NAME_LEN))) )
  ,ConsumerID    ( 0 )
  ,workThreadID  ( 0 )
  ,commThreadID  ( 0 )
  ,startTime     ( time(NULL) )
  ,inQueue       ( strcat(strncpy( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) ), "_in"), 10, sizeof(TCommand), true )
  ,outQueue      ( strcat(strncpy( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) ), "_out"), 10, sizeof(TCommand), true )
  ,initAttempt   ( 0 )
{
   strncpy( pcDrvName, const_cast<char*>(pcName), sizeof(pcDrvName) );
   strcat(strncpy( pcCommThreadName, const_cast<char*>(pcName), sizeof(pcDrvName) ), "_com");
}

//------------------------------------------------------------------------------

IxDriver::~IxDriver()
{
   task_delete();
   comm_task_delete();

   printDebug("IxDriver/%s: driver=%s deleted", __FUNCTION__, pcDrvName);
}

//------------------------------------------------------------------------------

void IxDriver::task_run( )
{
   create_thread();
   create_comm_thread();
}
//------------------------------------------------------------------------------

uint64_t IxDriver::get_time()
{
   return static_cast<uint64_t>(difftime(time(NULL), startTime));
}

//------------------------------------------------------------------------------

int32_t IxDriver::create_thread( )
{
   int32_t task_result = 0;
   pthread_attr_t attr;

   pthread_attr_init(&attr);
   int s = pthread_attr_setstacksize(&attr, 1048576);
   if (s != 0)
   {
      printError("IxRunnable/%s: thread=%s pthread_attr_setstacksize error!!!", __FUNCTION__, pcDrvName);
   }
   task_result = pthread_create(&workThreadID, &attr, thRunnableFunction_IxDriver, this);

   if (task_result != 0)
   {
      printError("IxDriver/%s: driver=%s error!!!", __FUNCTION__, pcDrvName);
   }
   else
   {
      pthread_setname_np(workThreadID, pcDrvName);
   }

   return task_result;
}

int32_t IxDriver::create_comm_thread( )
{
   int32_t task_result = 0;
   pthread_attr_t attr;

   pthread_attr_init(&attr);
   int s = pthread_attr_setstacksize(&attr, 1048576);
   if (s != 0)
   {
      printError("IxRunnable/%s: thread=%s pthread_attr_setstacksize error!!!", __FUNCTION__, pcCommThreadName);
   }

   task_result = pthread_create(&commThreadID, &attr, thRunnableCommFunction_IxDriver, this);

   if (task_result != 0)
   {
      printError("IxDriver/%s: comm thread =%s error!!!", __FUNCTION__, pcCommThreadName);
   }
   else
   {
      pthread_setname_np(commThreadID, pcCommThreadName);
   }

   return task_result;
}

//------------------------------------------------------------------------------

void IxDriver::task_delete( )
{
   if (workThreadID != 0)
   {
      pthread_join(workThreadID, NULL);

      pthread_cancel(workThreadID);

      printDebug("IxDriver/%s: thread=%s deleted", __FUNCTION__, pcDrvName);
   }
}

void IxDriver::comm_task_delete( )
{
   if (commThreadID != 0)
   {
      pthread_cancel(commThreadID);

      printDebug("IxDriver/%s: thread=%s deleted", __FUNCTION__, pcCommThreadName);
   }
}

//------------------------------------------------------------------------------

void IxDriver::run()
{
  while(true)
  {
    DrvProcessor();
	sleep_mcs(10);
  }
}

void IxDriver::run_comm()
{
  while(true)
  {
    CommProcessor();
	 sleep_mcs(10);
  }
}

void * IxDriver::thRunnableFunction_IxDriver( void *args )
{
   (reinterpret_cast<IxDriver*>(args))->run();
   return 0;
}

void * IxDriver::thRunnableCommFunction_IxDriver( void *args )
{
   (reinterpret_cast<IxDriver*>(args))->run_comm();
   return 0;
}

//------------------------------------------------------------------------------

void IxDriver::DrvProcessor()
{
   // if driver was initialised - call thread processor
   if (initAttempt > 0)
   {
      ThreadProcessor( );
   }
}

void IxDriver::CommProcessor( )
{
   TCommand Command = { 0, 0, 0, 0, NULL };

   if (-1 != inQueue.receive(reinterpret_cast<void*>(&Command), sizeof(TCommand)))
   {
      if (Command.ConsumerID == DrvID)
      {
         // printDebug("IxDriver/%s: ConsumerID=%d, SenderID=%d,ComType=%d, ComID=%d ", __FUNCTION__, Command.ConsumerID, Command.SenderID, Command.ComType, Command.ComID);

         // command is mine
         if ((Command.ComType == identification_request) && (Command.ComID == CM_DIReq))
         {
            // remember current consumer
            ConsumerID = Command.SenderID;

            // set up resonce for top level driver
            Command.ConsumerID = Command.SenderID;
            Command.SenderID   = DrvID;
            Command.ComType    = identification_response;
            Command.ComID      = CM_DIRes;

            outQueue.send( reinterpret_cast<const void*>(&Command), sizeof(TCommand) );

            initAttempt++;
         }
         else
         {
            // we should check was client registred or not
            if (0 != ConsumerID)
            {
               CommandProcessor( Command.ComID, Command.Container );
            }
            else
            {
               printWarning("IxDriver/%s: unexpected msg ComID=%d, ConsumerID=%d, SenderID=%d ", __FUNCTION__, Command.ComID, Command.ConsumerID, Command.SenderID);
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void IxDriver::ThreadProcessor( )
{
   workThreadID = 0;
   printError("IxDriver/%s: thread=%s not implemented, EXIT!!!", __FUNCTION__, pcDrvName);
   pthread_exit(0);
}

void IxDriver::sendMsg( uint16_t ComID, void *data )
{
   TCommand Command = { 0, 0, 0, 0, NULL };

   // we should check was client registred or not
   if (0 != ConsumerID)
   {
      // set up resonce for top level driver
      Command.ConsumerID = ConsumerID;
      Command.SenderID   = DrvID;
      Command.ComType    = response;
      Command.ComID      = ComID;
      Command.Container  = data;

      outQueue.send( reinterpret_cast<const void*>(&Command), sizeof(TCommand) );
   }
}

