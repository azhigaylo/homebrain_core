#include <iostream>
#include "utils.h"
#include "slog.h"

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

using namespace std;
using namespace event_pool;


//------------------------------------------------------------------------------

class EvenREC : public IxEventConsumer
{
   public:
      EvenREC();
      ~EvenREC(){}

      virtual bool processEvent( pTEvent pEvent );
	  
	  int event_counter;
};

EvenREC::EvenREC() : IxEventConsumer(), event_counter(0)
{
   setNotification( event_pool::EVENT_POWER_ON );
}

bool EvenREC::processEvent( pTEvent pEvent )
{
   if (pEvent->eventType == event_pool::EVENT_POWER_ON )
   {
	  event_counter++;
      printDebug("HomeBrainVx01/%s: got event_pool::EVENT_POWER_ON, event_counter=%i !!! ", __FUNCTION__, event_counter );
   }
   
   return true;
}

//------------------------------------------------------------------------------

int main()
{
    printDebug("HomeBrainVx01/%s: Staaaaaaaaaaaaaaaaaaart !!!! ", __FUNCTION__);
    // -------------------------------------------------------------------------------------------------------
	IxDriver driver( "driver" );
	driver.task_run();
    // -------------------------------------------------------------------------------------------------------
    CxThreadIO threadIO( "iothread", "driver" );
	threadIO.Start();
    // -------------------------------------------------------------------------------------------------------
	CxLogDevice LogDevice_1("LogDevice_1");
	CxLogDevice LogDevice_2("LogDevice_2");
    // -------------------------------------------------------------------------------------------------------
	CxInterface Interface_1("Interface_1");
	CxInterface Interface_2("Interface_2");	
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
    EvenREC eventTST;
    // -------------------------------------------------------------------------------------------------------

	while(1)
	{
	   sleep_mcs(1);
	   
	}
}