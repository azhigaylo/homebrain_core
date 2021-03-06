/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_LAUNCHER
#define _CX_LAUNCHER

//------------------------------------------------------------------------------
#include <signal.h>
#include <vector>
//------------------------------------------------------------------------------
#include "os_wrapper/IxRunnable.h"
#include "os_wrapper/IxDriver.h"

#include "processor/CxUsoProcessor.h"
#include "eventpool/IxEventConsumer.h"
#include "parser/CxIniFileParser.h"
#include "devctrl/CxLogDeviceManager.h"
#include "interface/CxInterfaceManager.h"

#include "CxUsoCfgLoader.h"
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
      virtual ~CxLauncher();

   private:
      // current state os launcher
      TLauncherState LauncherState;
      // interface manager
      const CxInterfaceManager *pInterfaceManager;
      // logical device manager
      const CxLogDeviceManager *pLogDeviceManager;
      // state of the connection to data server
      bool bPowerReady;
      // ini file parcer object
      CxIniFileParser IniFileParser;
      // project loader
      CxUsoCfgLoader UsoCfgLoader;
      // list for each uso processors
      std::vector<CxUsoProcessor*> ProcessorList;
      // list for every driver in the system
      std::vector<IxDriver*> DriverList;

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
      // start uso processors
      void startUsoProcessors();
      // close all activities
      void close_activities();

      // FSM process
      virtual void TaskProcessor( );

      virtual bool processEvent( pTEvent pEvent );     // form IxEventConsumer

      char cgfname[configMAX_NAME_NAME_LEN];

 }; typedef CxLauncher *pCxLauncher;

#endif // _CX_LAUNCHER
