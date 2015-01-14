#ifndef _CX_LAUNCHER
#define _CX_LAUNCHER

//------------------------------------------------------------------------------

#include "IxRunnable.h"
#include "CxDriverManager.h"
#include "..\eventpool\IxEventConsumer.h"
#include "..\devctrl\CxLogDeviceManager.h"
#include "..\connection\CxConnectionManager.h"
#include "..\parser\CxIniFileParser.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*

serve for:
 1. configuration of ports / units
 2. start system / debugging / devices threads
 3. put thread in sleep

*/

class CxLauncher : public IxRunnable, public IxEventConsumer
{   
  
  enum TStateDBG
  {
      dbg_off = 0,    
      dbg_on
  };        

  enum TLauncherState
  {
      ST_L_UNKNOWN = 0,
      ST_L_CONFIG,
      ST_L_DRIVERS_START,
      ST_L_SYS_THREAD_START,
      ST_L_SYS_WAIT_FFS_READY,
      ST_L_LOG_DEVICE_START,
      ST_L_SLEEP
  };
  
  public:  

     static CxLauncher& getInstance();
     
     // start all parts of system task
     void Start();

  private:       
    
     // function's   
     CxLauncher();       
     ~CxLauncher();
     
     // current state os launcher
     TLauncherState LauncherState;
     // driver manager
     CxDriverManager &DriverManager;
     // logical device manager
     CxLogDeviceManager &LogDeviceManager;
     // connection manager
     CxConnectionManager &ConnectionManager;
     // state of the main FFS
     bool bFfsReady;
     // ini file parcer object
     CxIniFileParser IniFileParser;    
          
     // load all drivers
     void load_all_drivers();
     // start all tasks
     void start_sys_threads();
     // start all tasks
     void start_all_logdev();
     
     // FSM process
     virtual void TaskProcessor( );

     virtual bool processEvent( pTEvent pEvent );                               // form IxEventConsumer
         
     // work with scheduler 
     void scheduler_start();
     void scheduler_stop();
     
 }; typedef CxLauncher *pCxLauncher;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

