//------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

#include "common/utils.h"
#include "eventpool/CxEventDispatcher.h"
#include "serial/CxSerialDriver.h"
#include "interfaces/CxModBusMaster.h"
#include "interfaces/CxModBusSlave.h"
#include "interfaces/CxDataAccessServer.h"
#include "devctrl/CxLogDeviceManager.h"

#include "startup/CxLauncher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

// load all drivers
void CxLauncher::load_debug()
{

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
      strncpy(serialName, name, 50 );
      char* path = IniFileParser.ReadString( cgfname, sDrvName, "path" );
      strncpy(serialPath, path, 50 );
      dcb.BaudRate = IniFileParser.ReadInt( cgfname, sDrvName, "baudrate", 115200 );
      dcb.Parity   = static_cast<uint8_t>(IniFileParser.ReadInt( cgfname, sDrvName, "parity", 0 ));

      printDebug("CxLauncher::%s: %s name = %s", __FUNCTION__, sDrvName, serialName);
      printDebug("CxLauncher::%s: %s path = %s", __FUNCTION__, sDrvName, serialPath);
      printDebug("CxLauncher::%s: %s baudrate = %d", __FUNCTION__, sDrvName, dcb.BaudRate);

      CxSerialDriver *pDriver = new CxSerialDriver( serialName, serialPath, &dcb );
      DriverList.push_back( pDriver );
      pDriver->driver_run();
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
         printDebug("CxLauncher::%s: sDriverItem = %s", __FUNCTION__, sDriverItem);
         load_driver( sDriverItem );
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

         printDebug("CxLauncher::%s: %s =  %s/%s/%s", __FUNCTION__, sIntName, sType, sName, sDriver);

         // MB_MASTER interfaces
         if( 0 == strcmp(sType, "mb_master"))
         {
            CxModBusMaster *pModBusMaster = new CxModBusMaster( sName, sDriver );        // this item will be deleted in CxInterfaceManager::delInstance()
            pModBusMaster->open();
            // for each user interface we will create thread for independent processing
            CxUsoProcessor *pUsoProcessor = new CxUsoProcessor( "uso_processor", sName );
            ProcessorList.push_back( pUsoProcessor );
         }

         // MB_SLAVE interfaces
         if( 0 == strcmp(sType, "mb_slave"))
         {
            CxModBusSlave *pModBusSlave = new CxModBusSlave( sName, sDriver );          // this item will be deleted in CxInterfaceManager::delInstance()
            pModBusSlave->open();
         }

         // POINT_SERVER interfaces
         if( 0 == strcmp(sType, "socket_server"))
         {
            CxDataServer *pDataServer = new CxDataServer( sName, 8096, "127.0.0.1" );  // this item will be deleted in CxInterfaceManager::delInstance()
            pDataServer->open();
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
   pCxLogDeviceManager pLogDeviceMan = CxLogDeviceManager::getInstance();

   for( uint8_t itr = 0; itr < ProcessorList.size(); itr++ )
   {
      CxUsoProcessor *pPrc = ProcessorList[itr];

      for (uint8_t indx = 0; ; indx++)
      {
         IxLogDevice *pLogDevice = pLogDeviceMan->get_logdev_by_number( indx );

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
   for( uint8_t itr = 0; itr < ProcessorList.size(); itr++ )
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

   // close uso processors
   printDebug("HomeBrainVx01/%s: close interface processors...", __FUNCTION__ );
   for( uint8_t itr = 0; itr < ProcessorList.size(); itr++ )
   {
      CxUsoProcessor *pPrc = ProcessorList[itr];

      if( NULL != pPrc )
      {
         delete pPrc;
      }
   }

   // here will be deleted not only LogDeviceManager, will be deleted all logical devices
   printDebug("HomeBrainVx01/%s: close logdevices and manager...", __FUNCTION__ );
   pCxLogDeviceManager pLogDeviceMan = CxLogDeviceManager::getInstance();
   pLogDeviceMan->delInstance();

   // here will be deleted not only InterfaceManager, will be deleted all interfaces
   printDebug("HomeBrainVx01/%s: close interfaces and manager...", __FUNCTION__ );
   pCxInterfaceManager pInterfaceMan = CxInterfaceManager::getInstance();
   pInterfaceMan->delInstance();

   // close drivers
   printDebug("HomeBrainVx01/%s: close drivers...", __FUNCTION__ );
   for( uint8_t itr = 0; itr < DriverList.size(); itr++ )
   {
       IxDriver *pDrv = DriverList[itr];

       if( NULL != pDrv )
       {
          delete pDrv;
       }
   }
   DriverList.clear();

   // close event pool
   printDebug("HomeBrainVx01/%s: close event processing...", __FUNCTION__ );
   pTCxEventDispatcher pEventDispatcher = CxEventDispatcher::getInstance();
   pEventDispatcher->delInstance();
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
  ,UsoCfgLoader      ()
  ,ProcessorList     ()
  ,DriverList        ()
{
   strncpy( cgfname, const_cast<char*>(cgf_name), sizeof(cgfname) );

   // set notification for data server
   setNotification( event_pool::EVENT_DATA_CONNECTED );
}

CxLauncher::~CxLauncher( )
{
   close_activities();
   printDebug("CxLauncher::%s: all activities closed...", __FUNCTION__);
}

void CxLauncher::Start()
{
   // create thread
   task_run( );
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
         printDebug("CxLauncher::%s: start all drivers...", __FUNCTION__);
         load_all_drivers( );                                                    // load all drivers
         LauncherState = ST_L_SYS_THREAD_START;                                  // put in next state
         break;
      }
      case ST_L_SYS_THREAD_START :
      {
         printDebug("CxLauncher::%s: start all interfaces...", __FUNCTION__);
         start_all_interface();
         LauncherState = ST_L_LOG_DEVICE_START;                                // put in next state
         break;
      }
      case ST_L_LOG_DEVICE_START :
      {
         printDebug("CxLauncher::%s: start all logical devices...", __FUNCTION__);
         start_all_logdev();
         printDebug("CxLauncher::%s: start uso processing...", __FUNCTION__);
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
      printDebug("CxLauncher::%s: CxLauncher DataConnectReady received", __FUNCTION__);
      bDataConnectReady = true;
      return true;
   }

   return false;
}
