#include "IxEventConsumer.h"
#include "CxEventDispatcher.h"

IxEventConsumer::IxEventConsumer()
{

}  // end of destructor


IxEventConsumer::~IxEventConsumer()
{

}  // end of destructor

bool IxEventConsumer::setNotification(CxEvent::EventType event)
{
    CxEventDispatcher &Dispatcher = CxEventDispatcher::getInstance();
    TEvent EventData; EventData.eventType = event;
    if(Dispatcher.setNotification(EventData,this))return true;
     else return false;
}

bool IxEventConsumer::clrNotification(CxEvent::EventType event)
{
    CxEventDispatcher &Dispatcher = CxEventDispatcher::getInstance();
    TEvent EventData; EventData.eventType = event;
    if(Dispatcher.clrNotification(EventData,this)) return true;
     else return false;
}
