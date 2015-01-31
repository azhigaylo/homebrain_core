#include <iostream>
#include "utils.h"
#include "slog.h"
#include "DebugMacros.h"
#include "CxLauncher.h"

using namespace std;
using namespace event_pool;

//------------------------------------------------------------------------------
/*
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
*/
//------------------------------------------------------------------------------

int main()
{
   // application start
   // all initialization is in the launcher class
   CxLauncher launcher("/home/azhigaylo/.config/home_brain/HBconfig.conf");
   launcher.Start();

   while(1)
   {
      sleep_mcs(1);
   }
}