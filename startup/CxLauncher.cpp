//------------------------------------------------------------------------------
#include "DebugMacros.h"
#include "ScopeDeclaration.h"

#include "CxSerialDriver.h"
#include "CxModBusMaster.h"
#include "CxModBusSlave.h"
#include "CxDataConnection.h"
#include "CxEventDispatcher.h"
#include "CxLogDeviceManager.h"

#include "CxLauncher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

// load all drivers
void CxLauncher::load_debug()
{
   bool dbgState = IniFileParser.ReadBool( cgfname, "DEBUG", "state", false );

   if (true == dbgState)
   {
      ScopeRegistration();
   }
}

//------------------------------------------------------------------------------

void CxLauncher::load_driver( const char *sDrvName )
{
   // start serial driver SERIAL_1
   bool portState = IniFileParser.ReadBool( cgfname, sDrvName, "state", false );

   if (true == portState)
   {
      char serialName[50];
      char serialPath[50];   
      DCB dcb = {115200, 0, 8, 1};

      char* name = IniFileParser.ReadString( cgfname, sDrvName, "name" );
      strncpy_m(serialName, name, 50 );
      char* path = IniFileParser.ReadString( cgfname, sDrvName, "path" );
      strncpy_m(serialPath, path, 50 );
      dcb.BaudRate = IniFileParser.ReadInt( cgfname, sDrvName, "baudrate", 115200 );
      dcb.Parity   = IniFileParser.ReadInt( cgfname, sDrvName, "parity", 0 );

      printDebug("CxLauncher/%s: %s name = %s", __FUNCTION__, sDrvName, serialName);
      printDebug("CxLauncher/%s: %s path = %s", __FUNCTION__, sDrvName, serialPath);
      printDebug("CxLauncher/%s: %s baudrate = %d", __FUNCTION__, sDrvName, dcb.BaudRate);

      CxSerialDriver *pDriver = new CxSerialDriver( serialName, serialPath, &dcb );
      pDriver->task_run();
   }

}
// load all drivers
void CxLauncher::load_all_drivers()
{
   char* sDrivers = IniFileParser.ReadString( cgfname, "DRIVER", "list" );

   if (0 != sDrivers)
   {
      sDrivers = strdup(sDrivers);
      char* sDriverItem = strsep(&sDrivers, " ");

      do
      {
         load_driver( sDriverItem );
         printDebug("CxLauncher/%s: sDriverItem = %s", __FUNCTION__, sDriverItem);
         sDriverItem = strsep(&sDrivers, " ");
      }
      while( NULL != sDriverItem );

      free(sDrivers);
   }
}

//------------------------------------------------------------------------------

void CxLauncher::start_sys_interface( const char *sIntName )
{
   if ( NULL != sIntName )
   {
      bool intState = IniFileParser.ReadBool( cgfname, const_cast<char*>(sIntName), "state", false );

      if (true == intState)
      {
         char* sType = IniFileParser.ReadString( cgfname, const_cast<char*>(sIntName), "type" );
         sType = strdup(sType);

         char* sName = IniFileParser.ReadString( cgfname, const_cast<char*>(sIntName), "name" );
         sName = strdup(sName);

         char* sDriver = IniFileParser.ReadString( cgfname, const_cast<char*>(sIntName), "driver" );
         sDriver = strdup(sDriver);

         printDebug("CxLauncher/%s: %s =  %s/%s/%s", __FUNCTION__, sIntName, sType, sName, sDriver);

         // MB_MASTER interfaces
         if( 0 == strcmp(sType, "mb_master"))
         {
            CxModBusMaster *pModBusMaster = new CxModBusMaster( sName, sDriver );        // this item will be deleted in CxInterfaceManager::delInstance()
            pModBusMaster->open();
            // for each user interface we will create thread for independent processing
            CxUsoProcessor *pUsoProcessor = new CxUsoProcessor( "uso_processor", sName );
            ProcessorList.add( pUsoProcessor );
         }

         // MB_SLAVE interfaces
         if( 0 == strcmp(sType, "mb_slave"))
         {
            CxModBusSlave *pModBusSlave = new CxModBusSlave( sName, sDriver );          // this item will be deleted in CxInterfaceManager::delInstance()
            pModBusSlave->open();
         }

         // DATA_CNCT interfaces
         if( 0 == strcmp(sType, "dtaconnect"))
         {
            CxDataConnection *pDataConnection = new CxDataConnection(sName, sDriver );   // this item will be deleted in CxInterfaceManager::delInstance()
            pDataConnection->open();
         }

         free(sType);
         free(sName);
         free(sDriver);
      }
   }
}

// start all tasks
void CxLauncher::start_all_interface()
{
   char* sInterfaces = IniFileParser.ReadString( cgfname, "INTERFACE", "list" );

   if (0 != sInterfaces)
   {
      sInterfaces = strdup(sInterfaces);
      char* sInterfaceItem = strsep(&sInterfaces, " ");

      do
      {
         start_sys_interface( sInterfaceItem );
         sInterfaceItem = strsep(&sInterfaces, " ");
      }
      while( NULL != sInterfaceItem );

      free(sInterfaces);
   }
}

//------------------------------------------------------------------------------

void CxLauncher::start_all_logdev()
{
   char* sConfigPath = IniFileParser.ReadString( cgfname, "USOCONFIG", "path" );

   if (0 != sConfigPath)
   {
      sConfigPath = strdup(sConfigPath);

      UsoCfgLoader.Load(sConfigPath);

      free(sConfigPath);
   }
}

//------------------------------------------------------------------------------

// start uso processors
void CxLauncher::startUsoProcessors()
{
   pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();
         
   for( uint8_t itr = 0; itr < ProcessorList.count(); itr++ )
   {
      CxUsoProcessor *pPrc = ProcessorList[itr];

      for (uint8_t indx = 0; ; indx++)
      {
         IxLogDevice *pLogDevice = pLogDeviceManager->get_logdev_by_number( indx );

         if ((pLogDevice != 0) && (0 != pPrc))
         {
            if( 0 == strcmp( pPrc->get_interfacename(), pLogDevice->getInterfaceName() ) )
            {
               pPrc->set_logdev( pLogDevice );
            }
         }
         else
         {
            break;
         }
      }
   }

   // start all tasks
   for( uint8_t itr = 0; itr < ProcessorList.count(); itr++ )
   {
      CxUsoProcessor *pPrc = ProcessorList[itr];

      if( NULL != pPrc )
      {
         pPrc->task_run();
      }
   }
}
      
//------------------------------------------------------------------------------

// close all tasks
void CxLauncher::close_activities()
{
   printDebug("HomeBrainVx01/%s: close_activities...", __FUNCTION__ );

   // delete my own task
   task_delete( );
   
   // close uso processors
   for( uint8_t itr = 0; itr < ProcessorList.count(); itr++ )
   {
      CxUsoProcessor *pPrc = ProcessorList[itr];

      if( NULL != pPrc )
      {
         delete pPrc;
      }
   }
   // here will be deleted not only InterfaceManager, will be deleted all interfaces
   pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();
   pInterfaceManager->delInstance();

   // here will be deleted not only LogDeviceManager, will be deleted all logical devices
   pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();
   pLogDeviceManager->delInstance();

   pTCxEventDispatcher pEventDispatcher = CxEventDispatcher::getInstance();
   pEventDispatcher->delInstance();

   pTCxDebugBase pDebugBase = CxDebugBase::getInstance();
   pDebugBase->delInstance();

   pTCxStaticPool pStaticPool = CxStaticPool::getInstance();
   pStaticPool->delInstance();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CxLauncher::CxLauncher( const char* cgf_name ):
   IxRunnable        ( "INI_TASK"   )
  ,LauncherState     ( ST_L_UNKNOWN )
  ,pInterfaceManager ( CxInterfaceManager::getInstance() )  
  ,pLogDeviceManager ( CxLogDeviceManager::getInstance() ) 
  ,bDataConnectReady ( false )
  ,IniFileParser     ( )
  ,ProcessorList     ( 5 )
{
   strncpy_m ( cgfname, const_cast<char*>(cgf_name), sizeof(cgfname) );  

   // set notification for data server
   setNotification( event_pool::EVENT_DATA_CONNECTED );
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
         start_all_interface();
         LauncherState = ST_L_SYS_WAIT_CONNECTION;                                // put in next state
         break;
      }
      case ST_L_SYS_WAIT_CONNECTION:
      {
         if( bDataConnectReady == true )
         {
            LauncherState = ST_L_LOG_DEVICE_START;                                // put in next state
         }
         else
         {
            sleep_mcs(10);
         }
         break;
      }
      case ST_L_LOG_DEVICE_START :
      {
         printDebug("CxLauncher/%s: start all interface and logical device...", __FUNCTION__);
         start_all_logdev();
         startUsoProcessors();

         LauncherState = ST_L_NORMAL_WORK;                                             // put in next state
         break;
      }
      case ST_L_NORMAL_WORK :
      {
         sleep_mcs(200000);
         break;
      }

      default : break;
   }
}

bool CxLauncher::processEvent( pTEvent pEvent )
{
   // value event processing  
   if( pEvent->eventType == event_pool::EVENT_DATA_CONNECTED )
   {
      printDebug("CxLauncher/%s: CxLauncher DataConnectReady received", __FUNCTION__);
      bDataConnectReady = true;  
      return true;
   }

   return false;
}
