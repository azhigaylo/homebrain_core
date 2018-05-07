#include <iostream>
#include <getopt.h>
#include <signal.h>

#include "common/slog.h"
#include "common/utils.h"

#include "startup/CxLauncher.h"

using namespace std;
using namespace event_pool;

//------------------------------------------------------------------------------
static volatile bool terination_flag = false;
//------------------------------------------------------------------------------

void mainSigHandler( int /*sig*/ )
{
   printWarning("\nMAIN/%s: exit request detected...", __FUNCTION__);

   terination_flag = true;
}

int main(int argc, char* argv[])
{
   // establish handler for SIGTERM signal
   signal(SIGINT, mainSigHandler);

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

       CxLauncher *pLauncher = new CxLauncher(sCfg);
       pLauncher->Start();

       while (!terination_flag){sleep_mcs(200000);}

       delete pLauncher;

       printDebug("MAIN/%s: Launcher thread finished...", __FUNCTION__);
   }
   catch (const std::exception& e)
   {
       printError("MAIN/%s: Error description:", __FUNCTION__, e.what());
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
