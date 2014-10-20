#ifndef _IX_EVENT_PRODUCER
#define _IX_EVENT_PRODUCER

#include "CxEvent.h"

//------------------------------------------------------------------------------
//----------------interface class for evrnt consumer objects--------------------
//------------------------------------------------------------------------------

class IxEventProducer  
{
 public:
   IxEventProducer();    
   virtual ~IxEventProducer();
   
   virtual bool sendEvent( CxEvent::EventType, void* ); 
   
 private:
}; // end of IxEventConsumer

#endif // _IX_EVENTCONSUMER_H

