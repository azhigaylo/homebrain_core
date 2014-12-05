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
  ,inQueue       ( strcat(strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) ), "_out"), 10, sizeof(TCommand) )
  ,outQueue      ( strcat(strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) ), "_in"),  10, sizeof(TCommand) )
{
   strncpy_m( pcDrvName, const_cast<char*>(drvName), sizeof(pcDrvName) );
} 

CxThreadIO::~CxThreadIO( )
{
   printDebug("CxThreadIO/%s: OI thread=%s deleted", __FUNCTION__, pcTaskName);
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
        if( true == CheckDrvCommand() )
        {
           threadIOState = ST_IO_NORMAL_WORK;             // put in next state        
        }  
        break;
      }	
      case ST_IO_NORMAL_WORK :
      {
        CheckDrvCommand();
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

   int32_t msg_s = outQueue.occupancy();

   if (msg_s > 0)
   {
      if (-1 != inQueue.receive(reinterpret_cast<void*>(&Command), sizeof(TCommand)))
      {
         printDebug("CxThreadIO/%s: ConsumerID=%d, SenderID=%d,ComType=%d, ComID=%d ", __FUNCTION__, Command.ConsumerID, Command.SenderID, Command.ComType, Command.ComID);
			
         if( Command.ConsumerID == threadID && Command.SenderID == drvID )
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

   return result;
}
   
//------------------------------------------------------------------------------
/*
#include "CxMutex.h"
uint16_t counter_item = 0;

void CxThreadIO::ThreadProcessor( )
{
   CxMutex m( "mux" );
   
   m.take();
   
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
*/