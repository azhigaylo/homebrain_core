//------------------------------------------------------------------------------
#include <stdlib.h>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "IxEventProducer.h"
#include "CxEventDispatcher.h"
//------------------------------------------------------------------------------

IxEventProducer::IxEventProducer()
{

}

IxEventProducer::~IxEventProducer()
{

}

bool IxEventProducer::sendEvent( eEventType event, void *eventContainer )
{
  CxEventDispatcher *pDispatcher = CxEventDispatcher::getInstance();
  
  TEvent EventData; 
  EventData.eventType = static_cast<unsigned short>(event);  
  EventData.eventData = eventContainer;
  
  if(pDispatcher->setEvent(EventData))return true;

  return false;  
}
