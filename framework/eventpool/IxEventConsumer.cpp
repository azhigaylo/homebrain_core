//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "IxEventConsumer.h"
#include "CxEventDispatcher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

IxEventConsumer::IxEventConsumer()
{

}  // end of destructor

IxEventConsumer::~IxEventConsumer()
{

}  // end of destructor

eEventType IxEventConsumer::getUnusedEvent( )
{
   CxEventDispatcher *pDispatcher = CxEventDispatcher::getInstance();
   
   return pDispatcher->getVirtualEvent();
}
	  
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

	