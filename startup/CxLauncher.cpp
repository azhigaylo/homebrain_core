//------------------------------------------------------------------------------
#include "CxLauncher.h"
//------------------------------------------------------------------------------
#include "IxRunnable.h"
#include "CxSysTimer.h"
#include "CxQueue.h"
#include "IxDriver.h"
#include "CxThreadIO.h"
#include "CxLogDevice.h"
#include "CxInterface.h"
#include "CxStaticPool.h"
#include "CxInterfaceManager.h"
#include "CxLogDeviceManager.h"
#include "IxEventConsumer.h"
#include "CxIniFileParser.h"
#include "DebugMacros.h"
#include "ScopeDeclaration.h"
#include "CxDebugBase.h"
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
//------------------------------------------------------------------------------
using namespace event_pool;

//------------------------------------------------------------------------------  
// load all drivers
void CxLauncher::load_all_drivers()
{

}

//------------------------------------------------------------------------------  
// start all tasks
void CxLauncher::start_sys_threads()
{
    // -------------------------------------------------------------------------------------------------------
    IxDriver *pDriver = new IxDriver( "driver" );
    pDriver->task_run();
    // -------------------------------------------------------------------------------------------------------
    CxThreadIO *pThreadIO = new CxThreadIO( "iothread", "driver" );
    pThreadIO->Start();
    // -------------------------------------------------------------------------------------------------------
    CxLogDevice *pLogDevice_1 = new CxLogDevice("LogDevice_1");
    CxLogDevice *pLogDevice_2 = new CxLogDevice("LogDevice_2");
    // -------------------------------------------------------------------------------------------------------
    CxInterface *pInterface_1 = new CxInterface("Interface_1");
    CxInterface *pInterface_2 = new CxInterface("Interface_2");	
    // -------------------------------------------------------------------------------------------------------
    pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();

    pIxInterface pInterface = pInterfaceManager->get_interface( "Interface_1" );
    printDebug("HomeBrainVx01/%s: find = %s interface ", __FUNCTION__, pInterface->getInterfaceName() );

    pInterface = pInterfaceManager->get_interface( "Interface_2" );
    printDebug("HomeBrainVx01/%s: find = %s interface ", __FUNCTION__, pInterface->getInterfaceName() );
    // -------------------------------------------------------------------------------------------------------
    pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();
  
    IxLogDevice *pLogDevice = pLogDeviceManager->get_logdev( "LogDevice_1" );
    printDebug("HomeBrainVx01/%s: find = %s logdev ", __FUNCTION__, pLogDevice->getDeviceName() );
  
    pLogDevice = pLogDeviceManager->get_logdev( "LogDevice_2" );
    printDebug("HomeBrainVx01/%s: find = %s logdev ", __FUNCTION__, pLogDevice->getDeviceName() );
    // -------------------------------------------------------------------------------------------------------
    //CxIniFileParser *pIniFileParser = new CxIniFileParser();
    //char *port_id = pIniFileParser->ReadString( "/home/azhigaylo/.config/home_brain/HBconfig.conf", "port", "port_id" );
    //printDebug("HomeBrainVx01/%s: port_id = %s", __FUNCTION__, port_id );
    //char *port_name = pIniFileParser->ReadString( "/home/azhigaylo/.config/home_brain/HBconfig.conf", "port", "port_name" );
    //printDebug("HomeBrainVx01/%s: port_name = %s", __FUNCTION__, port_name );

}

void CxLauncher::start_all_logdev()
{
/*
     // all units
     char bLogDevNumber = 0;
     pCxLogDevice pLogdev = LogDeviceManager.getLogDev( bLogDevNumber );
   
     while( NULL != pLogdev )
     {
       char* devName = pLogdev->GetDeviceName( );
       
       if( pLogdev != NULL )
       {
         // read device availability
         bool enable = IniFileParser.ReadBool( "/sys/config/config.ini", devName, "enable", 0 ); 
         
         // if everythings ok - make configuration
         if( enable != false )
         {   
            // get port name          
            char *port_name = IniFileParser.ReadString( "/sys/config/config.ini", devName, "port" );
            // set port properties
            unsigned long port_speed = static_cast<unsigned long>( IniFileParser.ReadLong("/sys/config/config.ini", port_name, "speed", 0) ); 
            char port_parity = static_cast<char>( IniFileParser.ReadLong("/sys/config/config.ini", port_name, "parity", 0) );
            port_name = IniFileParser.ReadString( "/sys/config/config.ini", devName, "port" );
            // configuration of debug unit          
            pLogdev->SetCommunicationPort( port_name, port_speed, port_parity );
            pLogdev->SwitchOn(); 
            pLogdev->Start();
         }         
       }
       pLogdev = LogDeviceManager.getLogDev( ++bLogDevNumber );       
     }  
*/	 
}

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
         ScopeRegistration();
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
         DBG_SCOPE( CxTstThread, CxTstThread )

         DBG_MSG( ("CxLauncher/%s: delete launcher task, only event will be processed...",__FUNCTION__) );
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
