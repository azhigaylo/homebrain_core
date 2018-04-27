#ifndef _IX_EVENT_PRODUCER
#define _IX_EVENT_PRODUCER

//------------------------------------------------------------------------------
//----------------interface class for evrnt consumer objects--------------------
//------------------------------------------------------------------------------
#include "CxEvent.h"
#include "EventDefinition.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

class IxEventProducer
{
   public:
      IxEventProducer();
      virtual ~IxEventProducer();

      virtual bool sendEvent( eEventType event, uint16_t containerSize, void *eventContainer );

   private:
};

#endif // _IX_EVENTCONSUMER_H

