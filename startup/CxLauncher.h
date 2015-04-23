#ifndef _CX_LAUNCHER
#define _CX_LAUNCHER

//------------------------------------------------------------------------------
#include <signal.h>
//------------------------------------------------------------------------------
#include "IxRunnable.h"
#include "CxUsoCfgLoader.h"
#include "IxEventConsumer.h"
#include "CxIniFileParser.h"
#include "CxLogDeviceManager.h"
#include "CxInterfaceManager.h"
//------------------------------------------------------------------------------
#define configMAX_NAME_NAME_LEN 200
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
      ST_L_SYS_WAIT_CONNECTION,
      ST_L_LOG_DEVICE_START,
      ST_L_NORMAL_WORK
   };

   public:

      // start all parts of system task
      void Start();

      CxLauncher( const char* cgf_name );
      ~CxLauncher();

   private:
      // current state os launcher
      TLauncherState LauncherState;
      // interface manager	  
      const CxInterfaceManager *pInterfaceManager;
      // logical device manager
      const CxLogDeviceManager *pLogDeviceManager;
      // state of the connection to data server
      bool bDataConnectReady;
      // ini file parcer object
      CxIniFileParser IniFileParser;
      // project loader
      CxUsoCfgLoader UsoCfgLoader;

      void load_driver( const char *sDrvName );
      void start_sys_interface( const char *sIntName );

      // load all debug scopes
      void load_debug();
      // load all drivers
      void load_all_drivers();
      // start all tasks
      void start_all_logdev();
      // start all interface
      void start_all_interface();

      // close all activities
      void close_activities();

      // FSM process
      virtual void TaskProcessor( );

      virtual bool processEvent( pTEvent pEvent );     // form IxEventConsumer

      //static void sigHandler( int sig );

      // work with scheduler 
      void scheduler_start(){}                         // only for RTOS 
      void scheduler_stop(){}                          // only for RTOS 

      char cgfname[configMAX_NAME_NAME_LEN];

 }; typedef CxLauncher *pCxLauncher;

#endif // _CX_LAUNCHER