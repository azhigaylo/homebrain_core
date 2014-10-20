#include "IxEventProducer.h"
#include "CxEventDispatcher.h"


//------------------------------------------------------------------------------ 

void EVENT_GEN( CxEvent::EventType event, void *eventContainer )
{
  static IxEventProducer Eventer;
  Eventer.sendEvent(event, eventContainer);
}

//------------------------------------------------------------------------------ 

IxEventProducer::IxEventProducer()
{

}

IxEventProducer::~IxEventProducer()
{

}  // end of destructor

bool IxEventProducer::sendEvent( CxEvent::EventType event, void *eventContainer )
{
  CxEventDispatcher &Dispatcher = CxEventDispatcher::getInstance();
  TEvent EventData; 
  EventData.eventType = static_cast<unsigned short>(event);  
  EventData.eventData = eventContainer;
  if(Dispatcher.EventPool.setEvent(EventData))return true;
    return false;  
}
