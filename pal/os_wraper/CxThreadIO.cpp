//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxThreadIO.h"
//------------------------------------------------------------------------------

CxThreadIO::CxThreadIO(  const char *taskName,  const char *drvName ):
   IxRunnable    ( taskName )
  ,threadIOState ( ST_IO_UNKNOWN )
  ,drvID         ( CRC16_T(const_cast<char*>(drvName), strlen_m(const_cast<char*>(drvName), configMAX_DRIVER_NAME_LEN)) )
  ,threadID      ( CRC16_T(const_cast<char*>(taskName), strlen_m(const_cast<char*>(taskName), configMAX_TASK_NAME_LEN)) )  
  ,commThreadID  ( 0 )
  ,inQueue       ( strcat(strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) ), "_out"), 10, sizeof(TCommand), true )
  ,outQueue      ( strcat(strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) ), "_in"),  10, sizeof(TCommand), true )
  ,initAttempt   ( 0 )
{
   strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) );
   strcat(strncpy_m( pcCommThreadName, const_cast<char*>(drvName), sizeof(pcCommThreadName) ), "_com");
} 

CxThreadIO::~CxThreadIO( )
{
   comm_task_delete();
   printDebug("CxThreadIO/%s: OI thread=%s deleted", __FUNCTION__, pcTaskName);
}

//------------------------------------------------------------------------------

void CxThreadIO::Start()
{ 
  // create thread
  create_comm_thread();
  
  task_run( ); 
}

//------------------------------------------------------------------------------

int32_t CxThreadIO::create_comm_thread( )
{
   int32_t task_result = 0;

   task_result = pthread_create(&commThreadID, NULL, thRunnableCommFunction_ThreadIO, this);

   if (task_result != 0)
   {
      printError("CxThreadIO/%s: comm thread=%s error!!!", __FUNCTION__, pcCommThreadName);
   }
   else
   {
      pthread_setname_np(commThreadID, pcCommThreadName);
   }

   return task_result;
}

void *CxThreadIO::thRunnableCommFunction_ThreadIO( void *args )
{   
   (reinterpret_cast<CxThreadIO*>(args))->run_comm();
}

void CxThreadIO::run_comm()
{
  while(true)
  {   
    CheckDrvCommand();
  }
} 

void CxThreadIO::comm_task_delete( )
{  
   if (commThreadID != 0)
   {
      pthread_cancel(commThreadID);

      printDebug("CxThreadIO/%s: thread=%s deleted", __FUNCTION__, pcCommThreadName);
   }
}

//------------------------------------------------------------------------------

void CxThreadIO::TaskProcessor()
{
   switch( threadIOState )
   {
      case ST_IO_UNKNOWN :
      {
        threadIOState = ST_IO_DRV_REQUEST;                // put in next state
        break;
      }
      case ST_IO_DRV_REQUEST :
      {
        DriverIdentificationRequest();
        threadIOState = ST_IO_WAIT_DRV_RESP;              // put in next state
        break;
      }
      case ST_IO_WAIT_DRV_RESP :
      { 
        if( initAttempt > 0 )
        {
           threadIOState = ST_IO_NORMAL_WORK;             // put in next state        
        }  
        break;
      }	
      case ST_IO_NORMAL_WORK :
      {
        ThreadProcessor();
        sleep_mcs(10);
        break;
      }
      default : break;
   } 
}

//------------------------------------------------------------------------------

void CxThreadIO::DriverIdentificationRequest()
{
   TCommand Command = { threadID, drvID, identification_request, DIReq, NULL };
   outQueue.send(reinterpret_cast<const void*>(&Command), sizeof(TCommand));
}

bool CxThreadIO::CheckDrvCommand()
{
   bool result = false;
   TCommand Command = { 0, 0, 0, 0, NULL };

   if (-1 != inQueue.receive(reinterpret_cast<void*>(&Command), sizeof(TCommand)))
   {
      printDebug("CxThreadIO/%s: ConsumerID=%d, SenderID=%d,ComType=%d, ComID=%d ", __FUNCTION__, Command.ConsumerID, Command.SenderID, Command.ComType, Command.ComID);

      if( Command.ConsumerID == threadID && Command.SenderID == drvID )
      {
         // command is mine
         if( (Command.ComType == identification_response) && (Command.ComID == DIRes) )
         {
            initAttempt++;
         }
         else
         {
            CommandProcessor( Command );
         }
      }
   }

   return result;
}
   
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
void CxThreadIO::ThreadProcessor( )
{
   workThreadID = 0;
   printError("IxDriver/%s: thread=%s not implemented, EXIT!!!", __FUNCTION__, pcDrvName);
   pthread_exit(0);
}
*/

#include "CxMutex.h"
uint16_t counter_item = 0;

void CxThreadIO::ThreadProcessor( )
{
   counter_item++;
   
   TCommand Command = { 0, 0, 0, 0, NULL };
   // set up resonce for top level driver
   Command.ConsumerID = 1895;  
   Command.SenderID   = 11857;               
   Command.ComType    = 12;
   Command.ComID      = counter_item;

   outQueue.send(reinterpret_cast<const void*>(&Command), sizeof(TCommand)); 

   sleep_s(1);
}
