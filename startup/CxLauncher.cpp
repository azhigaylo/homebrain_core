//------------------------------------------------------------------------------
#include "CxThreadIO.h"
#include "CxSerialDriver.h"
#include "DebugMacros.h"
#include "ScopeDeclaration.h"

#include "CxLauncher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
/*	
//------------------------------------------------------------------------------
CxLogDevice *pLogDevice_1 = new CxLogDevice("LogDevice_1");
CxLogDevice *pLogDevice_2 = new CxLogDevice("LogDevice_2");
//------------------------------------------------------------------------------
CxInterface *pInterface_1 = new CxInterface("Interface_1");
CxInterface *pInterface_2 = new CxInterface("Interface_2");	
//------------------------------------------------------------------------------
pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();

pIxInterface pInterface = pInterfaceManager->get_interface( "Interface_1" );
printDebug("HomeBrainVx01/%s: find = %s interface ", __FUNCTION__, pInterface->getInterfaceName() );

pInterface = pInterfaceManager->get_interface( "Interface_2" );
printDebug("HomeBrainVx01/%s: find = %s interface ", __FUNCTION__, pInterface->getInterfaceName() );
//------------------------------------------------------------------------------
pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();

IxLogDevice *pLogDevice = pLogDeviceManager->get_logdev( "LogDevice_1" );
printDebug("HomeBrainVx01/%s: find = %s logdev ", __FUNCTION__, pLogDevice->getDeviceName() );

pLogDevice = pLogDeviceManager->get_logdev( "LogDevice_2" );
printDebug("HomeBrainVx01/%s: find = %s logdev ", __FUNCTION__, pLogDevice->getDeviceName() );
//------------------------------------------------------------------------------
*/	

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

}

// start all tasks
void CxLauncher::start_sys_threads()
{   
   DCB dcb = {9600, 0, 8, 1};
   
   CxSerialDriver *pDriver = new CxSerialDriver( "driver", "/dev/ttyUSB0", &dcb );
   pDriver->task_run();

   CxThreadIO *pThreadIO = new CxThreadIO( "iothread", "driver" );
   pThreadIO->Start();
}

void CxLauncher::start_all_logdev()
{
 
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
