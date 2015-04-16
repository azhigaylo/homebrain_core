#include <iostream>
#include "utils.h"
#include "slog.h"
#include <getopt.h>

#include "DebugMacros.h"
#include "CxLauncher.h"

using namespace std;
using namespace event_pool;

//------------------------------------------------------------------------------
#include "CxLogDeviceManager.h"

int main(int argc, char* argv[])
{
   int c = 0;
   int delay = 100;
   char *sCfg = "/home/azhigaylo/.config/home_brain/HBconfig.conf";
   
   while (-1 != (c = getopt(argc, argv, "c:d:")))
   {
      switch (c)
      {
         case 'c':
         {
            // all initialization is in the launcher class
            sCfg = optarg;
            break;
         }
         case 'd':
         {
            delay = atoi(optarg);

            break;
         }
         default:
         {
            break;
         }
      }
   }

   printDebug("MAIN/%s: started with cfg = %s", __FUNCTION__, sCfg);
   CxLauncher launcher(sCfg);
   launcher.Start();

   printDebug("MAIN/%s: started with delay = %i", __FUNCTION__, delay);
   sleep_mcs(delay);   // 500000

   while(1)
   {
      pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();

      IxLogDevice *pLogDevice_MA = pLogDeviceManager->get_logdev( "LogDev_MA" );
      IxLogDevice *pLogDevice_EXTM = pLogDeviceManager->get_logdev( "LogDev_EXTM" );

      if (pLogDevice_MA){ pLogDevice_MA->Process();}
      //  else printDebug("MAIN/%s: ahtung pLogDevice_MA=%i!!!!!", __FUNCTION__, pLogDevice_MA);

      if (pLogDevice_EXTM){ pLogDevice_EXTM->Process();}
      //  else printDebug("MAIN/%s: ahtung pLogDevice_EXTM=%i!!!!!", __FUNCTION__, pLogDevice_EXTM);   
   }
}