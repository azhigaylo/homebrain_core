//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "eventpool/IxEventConsumer.h"
#include "eventpool/CxEventDispatcher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

IxEventConsumer::IxEventConsumer()
{

}  // end of destructor

IxEventConsumer::~IxEventConsumer()
{

}  // end of destructor

bool IxEventConsumer::setNotification(eEventType event)
{
   CxEventDispatcher *pDispatcher = CxEventDispatcher::getInstance();

   TEvent EventData;
   EventData.eventType = event;

   if(pDispatcher->setNotification(EventData,this))return true;

   return false;
}

bool IxEventConsumer::clrNotification(eEventType event)
{
   CxEventDispatcher *pDispatcher = CxEventDispatcher::getInstance();

   TEvent EventData;
   EventData.eventType = event;

   if(pDispatcher->clrNotification(EventData,this)) return true;

   return false;
}

