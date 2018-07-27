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
       int debug_sink  = 0;
       const char *sCfg = "/home/azhigaylo/.config/home_brain/HBconfig.conf";

       while (-1 != (c = getopt(argc, argv, "hc:d:s:")))
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
             case 's':
             {
                debug_sink = atoi(optarg);
                setDbgSink(debug_sink);
                break;
             }
             case 'h':
             {
                printf("-h : help\n");
                printf("-c : path to config\n");
                printf("-d : debug level 0-4(err/wr/info/dbg\n");
                printf("-s : debug sink 0-1(console/dlt\n");
                return EXIT_SUCCESS;
             }
             default:
             {
                printf("unsupported option...\n");
                return EXIT_SUCCESS;
             }
          }
       }

       if (TRACE_SINK_DLT == getDbgSink())
       {
           initDlt();
           printDebug("MAIN/%s: Dlt inited...", __FUNCTION__);
       }

       printDebug("MAIN/%s: started with debug level = %i", __FUNCTION__, debug);
       printDebug("MAIN/%s: started with debug sink = %i", __FUNCTION__, debug_sink);
       printDebug("MAIN/%s: started with cfg = %s", __FUNCTION__, sCfg);

       CxLauncher *pLauncher = new CxLauncher(sCfg);
       pLauncher->Start();

       while (!terination_flag){sleep_mcs(200000);}

       delete pLauncher;

       printDebug("MAIN/%s: Launcher thread finished...", __FUNCTION__);

       if (TRACE_SINK_DLT == getDbgSink())
       {
           deinitDlt();
           printDebug("MAIN/%s: Dlt deinited...", __FUNCTION__);
       }

   }
   catch (const std::exception& e)
   {
       printError("MAIN/%s: Error description:", __FUNCTION__, e.what());
       deinitDlt();
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

