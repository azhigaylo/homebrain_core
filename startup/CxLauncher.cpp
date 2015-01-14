//------------------------------------------------------------------------------

#include "CxLauncher.h"

#include "CxTstThread.h"
#include "..\vtimer\CxVtimer.h"
#include "..\eventpool\CxEventDispatcher.h"
#include "..\debug\DebugMacros.h"
#include "..\debug\ScopeDeclaration.h"

//------------------------------------------------------------------------------

CxLauncher::CxLauncher( ):
   IxRunnable        ( "INI_TASK"   )
  ,LauncherState     ( ST_L_UNKNOWN )
  ,DriverManager     ( CxDriverManager::getInstance()     )  
  ,LogDeviceManager  ( CxLogDeviceManager::getInstance()  ) 
  ,ConnectionManager ( CxConnectionManager::getInstance() )    
  ,bFfsReady         ( false )
  ,IniFileParser     ( )
{
   setNotification( CxEvent::EVENT_FFS_MOUNTED );
} 

CxLauncher::~CxLauncher( )
{

}

CxLauncher &CxLauncher::getInstance( )
{
  static CxLauncher theInstance;
  return theInstance;
}

void CxLauncher::Start()
{ 
  // create thread
  task_run( );
  // start RTOS scheduler
  scheduler_start();
}

//------------------------------------------------------------------------------  
// load all drivers
void CxLauncher::load_all_drivers()
{
  portENTER_CRITICAL();
  
    DriverManager.loadAllDrivers();

  portEXIT_CRITICAL();  
}

// start all tasks
void CxLauncher::start_sys_threads()
{
    // thread initialization    
    portENTER_CRITICAL();

      // timer base start
      CxTBase &tBase= CxTBase::getInstance();
      tBase.task_run();
      
      // event despatcher start
      CxEventDispatcher &dispatcher = CxEventDispatcher::getInstance();
      dispatcher.start();
      
      // activate all connection 
      ConnectionManager.activateAllConnection();
    
      // blinker thread
      CxTstThread &TstThread = CxTstThread::getInstance();   
      TstThread.Start();
      
    portEXIT_CRITICAL();
}

void CxLauncher::start_all_logdev()
{
  portENTER_CRITICAL();
  
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
     
  portEXIT_CRITICAL();     
}

//------------------------------------------------------------------------------
// FSM process
void CxLauncher::TaskProcessor()
{  
  DBG_SCOPE( CxLauncher, CxLauncher )   

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
        DBG_MSG( ("[M] System startup \n\r") );  
        DBG_MSG( ("[M] Scope list has been loaded \n\r") );           
        
        load_all_drivers( );                                                    // load all drivers
          
        LauncherState = ST_L_SYS_THREAD_START;                                  // put in next state
        break;
      }      
      case ST_L_SYS_THREAD_START :  
      { 
        start_sys_threads();
        task_sleep( 500 );
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
        start_all_logdev();       
        LauncherState = ST_L_SLEEP;                                             // put in next state        
        break;
      }	
      case ST_L_SLEEP :  
      { 
        task_suspend( );                                                        // suspend INI_PRC task
          
        LauncherState = ST_L_CONFIG;                                            // put in next state
        break;
      }	                              

      default : break;
   }
}

bool CxLauncher::processEvent( pTEvent pEvent )
{
  DBG_SCOPE( CxLauncher, CxLauncher )   
          
  // value event processing  
  if( pEvent->eventType == CxEvent::EVENT_FFS_MOUNTED )
  {        
    bFfsReady = true;  
    
    DBG_MSG( ("[M] CxLauncher-FFS ready\n\r") );
    return true;
  }
  
  return false;
}

void CxLauncher::scheduler_start()
{
   vTaskStartScheduler();
}

void CxLauncher::scheduler_stop()
{
   vTaskEndScheduler();
}
