#include <iostream>
#include <getopt.h>

#include "common/slog.h"
#include "common/utils.h"

#include "startup/CxLauncher.h"

using namespace std;
using namespace event_pool;

//------------------------------------------------------------------------------
CxLauncher *pLauncher = 0;
//------------------------------------------------------------------------------

void mainSigHandler( int /*sig*/ )
{
	printWarning("MAIN/%s: exit request detected...", __FUNCTION__);

   if (0 != pLauncher)
   {
      delete pLauncher;
      pLauncher = 0;
   }
}

int main(int argc, char* argv[])
{
   // establish handler for SIGTERM signal
   struct sigaction sa;
   sa.sa_handler = mainSigHandler;
   sigaction( SIGINT, &sa, 0);

   try
   {
       int c = 0;
       int debug = 0;
       const char *sCfg = "/home/azhigaylo/.config/home_brain/HBconfig.conf";

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
                debug = atoi(optarg);
                setDbgLevel(debug);
                break;
             }
             default:
             {
                break;
             }
          }
       }

       printDebug("MAIN/%s: started with debug level = %i", __FUNCTION__, debug);
       printDebug("MAIN/%s: started with cfg = %s", __FUNCTION__, sCfg);

       pLauncher = new CxLauncher(sCfg);
       pLauncher->Start();

       while (pLauncher){sleep_mcs(200000);}

       printDebug("MAIN/%s: Launcher thread finished...", __FUNCTION__);
   }
   catch (const std::exception& e)
   {
       printError("MAIN/%s: Error description:", __FUNCTION__, e.what());
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
