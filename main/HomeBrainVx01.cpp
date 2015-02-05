#include <iostream>
#include "utils.h"
#include "slog.h"
#include "DebugMacros.h"
#include "CxLauncher.h"

using namespace std;
using namespace event_pool;

//------------------------------------------------------------------------------
#include "CxLogDeviceManager.h"

int main()
{
   // application start
   // all initialization is in the launcher class
   CxLauncher launcher("/home/azhigaylo/.config/home_brain/HBconfig.conf");
   launcher.Start();

   while(1)
   {
      pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();
      IxLogDevice *pLogDevice = pLogDeviceManager->get_logdev( "LogDev_MA16" );

      if (pLogDevice){ pLogDevice->Process();}
        else printDebug("CxLogDev_MA16/%s: ahtung pLogDevice=%i!!!!!", __FUNCTION__, pLogDevice);   

      sleep_mcs(200000);
   }
}