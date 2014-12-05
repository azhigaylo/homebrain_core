#include <iostream>
#include "utils.h"
#include "IxRunnable.h"
#include "CxSysTimer.h"
#include "CxQueue.h"
#include "IxDriver.h"
#include "CxThreadIO.h"
#include "slog.h"

using namespace std;
  
int main()
{
/*
    IxRunnable task_1("task_1");
    CxSysTimer timer_1( "timer_1", 2000000000, true );

    task_1.task_run();	
	timer_1.Start();

	IxDriver driver( "driver" );
	driver.task_run();
	
	CxQueue inQueue  ( "driver_in",  10, sizeof(TCommand));
	CxQueue outQueue ( "driver_out", 10, sizeof(TCommand));
	
	TCommand Command = { 0, 0, 0, 0, NULL };
	// set up resonce for top level driver
	Command.ConsumerID = CRC16_T("driver", strlen_m("driver", configMAX_DRIVER_NAME_LEN));  
	Command.SenderID   = 1;               
	Command.ComType    = identification_request;
	Command.ComID      = DIReq;
	
	inQueue.send(reinterpret_cast<const void*>(&Command), sizeof(TCommand));   

	while(1)
	{
	   int32_t msg_s = outQueue.occupancy();
	   if (msg_s > 0)
	   {
	      if (-1 != outQueue.receive(reinterpret_cast<void*>(&Command), sizeof(TCommand)))
	      {
             printDebug("IxDriver/%s: msg_s=%d / ConsumerID=%d, SenderID=%d,ComType=%d, ComID=%d ", __FUNCTION__,msg_s, Command.ConsumerID, Command.SenderID, Command.ComType, Command.ComID);
	      }
       }		  
	}
*/
	IxDriver driver( "driver" );
	driver.task_run();

    CxThreadIO threadIO( "iothread", "driver" );
	threadIO.Start();
	
	while(1)
	{
	   sleep_mcs(1);
	}
}