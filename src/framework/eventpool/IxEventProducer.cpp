//------------------------------------------------------------------------------
#include <stdlib.h>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "eventpool/IxEventProducer.h"
#include "eventpool/CxEventDispatcher.h"
//------------------------------------------------------------------------------

IxEventProducer::IxEventProducer()
{

}

IxEventProducer::~IxEventProducer()
{

}

bool IxEventProducer::sendEvent( eEventType event, uint16_t containerSize, void *eventContainer )
{
  CxEventDispatcher *pDispatcher = CxEventDispatcher::getInstance();

  TEvent EventData;
  EventData.eventType = static_cast<unsigned short>(event);
  EventData.dataSize  = containerSize;
  EventData.eventData = eventContainer;

  if(pDispatcher->setEvent(EventData))return true;

  printError("CxDataProvider/%s: error send event with type = %d", __FUNCTION__, EventData.eventType);

  return false;
}
