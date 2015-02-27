//------------------------------------------------------------------------------
#include "DebugMacros.h"
#include "ScopeDeclaration.h"

#include "CxSerialDriver.h"
#include "CxModBusMaster.h"
#include "CxLogDev_MA16.h"

#include "CxLauncher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

// load all drivers
void CxLauncher::load_debug()
{
   bool dbgState = IniFileParser.ReadBool( "/home/azhigaylo/.config/home_brain/HBconfig.conf", "DEBUG", "state", false );

   if (true == dbgState)
   {
      ScopeRegistration();
   }
}

// load all drivers
void CxLauncher::load_all_drivers()
{
   // start serial driver SERIAL_1
   bool portState = IniFileParser.ReadBool( cgfname, "SERIAL_1", "state", false );

   if (true == portState)
   {
      char serialName[50];
      char serialPath[50];   
      DCB dcb = {115200, 0, 8, 1};

      char* name = IniFileParser.ReadString( cgfname, "SERIAL_1", "name" );
      strncpy_m(serialName, name, 50 ); 
      char* path = IniFileParser.ReadString( cgfname, "SERIAL_1", "path" );
      strncpy_m(serialPath, path, 50 ); 
      dcb.BaudRate = IniFileParser.ReadInt( cgfname, "SERIAL_1", "baudrate", 115200 );
      dcb.Parity   = IniFileParser.ReadInt( cgfname, "SERIAL_1", "parity", 0 );

      printDebug("CxLauncher/%s: SERIAL_1 name = %s", __FUNCTION__, serialName);
      printDebug("CxLauncher/%s: SERIAL_1 path = %s", __FUNCTION__, serialPath);
      printDebug("CxLauncher/%s: SERIAL_1 baudrate = %d", __FUNCTION__, dcb.BaudRate);

      CxSerialDriver *pDriver = new CxSerialDriver( serialName, serialPath, &dcb );
      pDriver->task_run();
   }

}

// start all tasks
void CxLauncher::start_sys_threads()
{

}

#include "USODefinition.h"

TAioChannel AI1_CH[4] = {
 {  0, ATYPE_AI_5mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 1, 5},    //
 {  1, ATYPE_AI_5mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 2, 6},    // 
 {  2, ATYPE_AI_5mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 3, 7},    // 
 {  3, ATYPE_AI_5mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 4, 8},    //  
}; 

void CxLauncher::start_all_logdev()
{
   CxModBusMaster *pModBusMaster = new CxModBusMaster( "mbus_master", "serial_1" );
   pModBusMaster->open();

   TContAI_USO contAI_USO = { 1, 1, 4, AI1_CH};
 
   CxLogDev_MA *pLogDev_MA = new CxLogDev_MA( "LogDev_MA", "mbus_master", contAI_USO);
}

// close all tasks
void CxLauncher::close_activities()
{
   printDebug("HomeBrainVx01/%s: close_activities...", __FUNCTION__ );
   
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pInterfaceManager->delInstance();

   pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();
   pLogDeviceManager->delInstance();

   pTCxStaticPool pStaticPool = CxStaticPool::getInstance();
   pStaticPool->delInstance();

   pTCxDebugBase pDebugBase = CxDebugBase::getInstance();
   pDebugBase->delInstance();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CxLauncher::CxLauncher( const char* cgf_name ):
   IxRunnable        ( "INI_TASK"   )
  ,LauncherState     ( ST_L_UNKNOWN )
  ,pInterfaceManager ( CxInterfaceManager::getInstance() )  
  ,pLogDeviceManager ( CxLogDeviceManager::getInstance() ) 
  ,bFfsReady         ( true )
  ,IniFileParser     ( )
{
   strncpy_m ( cgfname, const_cast<char*>(cgf_name), sizeof(cgfname) );  
   setNotification( event_pool::EVENT_FFS_MOUNTED );
   
   // establish handler for SIGTERM signal
   struct sigaction sa;
   sa.sa_handler = CxLauncher::sigHandler;
   sigaction(SIGTERM, &sa, 0);
} 

CxLauncher::~CxLauncher( )
{
   close_activities();
}

void CxLauncher::Start()
{ 
  // create thread
  task_run( );
  // start RTOS scheduler
  scheduler_start();
}

// FSM process
void CxLauncher::TaskProcessor()
{
   switch( LauncherState )
   {
      case ST_L_UNKNOWN :
      {
         LauncherState = ST_L_CONFIG;                                            // put in next state
         break;
      }
      case ST_L_CONFIG :
      {
         load_debug();
         LauncherState = ST_L_DRIVERS_START;                                     // put in next state
         break;
      }
      case ST_L_DRIVERS_START :
      {
         printDebug("CxLauncher/%s: System startup...", __FUNCTION__);
         load_all_drivers( );                                                    // load all drivers
         LauncherState = ST_L_SYS_THREAD_START;                                  // put in next state
         break;
      }
      case ST_L_SYS_THREAD_START :
      {
         printDebug("CxLauncher/%s: create all instances of classes...", __FUNCTION__);
         start_sys_threads();
         LauncherState = ST_L_SYS_WAIT_FFS_READY;                                // put in next state
         break;
      }
      case ST_L_SYS_WAIT_FFS_READY:
      {
         if( bFfsReady == true )
         {
            LauncherState = ST_L_LOG_DEVICE_START;                                // put in next state
         }
         break;
      }
      case ST_L_LOG_DEVICE_START :
      {
         printDebug("CxLauncher/%s: start all interface and logical device...", __FUNCTION__);
         start_all_logdev();
         LauncherState = ST_L_SLEEP;                                             // put in next state
         break;
      }
      case ST_L_SLEEP :
      {
          printDebug("CxLauncher/%s: delete launcher task, only event will be processed...", __FUNCTION__);
          task_delete( );
         break;
      }

      default : break;
   }
}

bool CxLauncher::processEvent( pTEvent pEvent )
{
   // value event processing  
   if( pEvent->eventType == event_pool::EVENT_FFS_MOUNTED )
   {
      bFfsReady = true;  
      printDebug("CxLauncher/%s: CxLauncher-FFS ready", __FUNCTION__);
      return true;
   }

   return false;
}

void CxLauncher::sigHandler( int sig )
{
   close_activities();
}

void CxLauncher::scheduler_start()
{

}

void CxLauncher::scheduler_stop()
{

}
