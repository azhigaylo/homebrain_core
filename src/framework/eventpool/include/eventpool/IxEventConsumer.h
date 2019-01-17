/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _IX_EVENT_CONSUMER
#define _IX_EVENT_CONSUMER
//------------------------------------------------------------------------------
//----------------interface class for evrnt consumer objects--------------------
//------------------------------------------------------------------------------
#include "CxEvent.h"
#include "EventDefinition.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------

class IxEventConsumer
{
   public:
      IxEventConsumer( );
      virtual ~IxEventConsumer( );

      virtual bool processEvent( pTEvent pEvent ) = 0;

      bool setNotification( eEventType event );
      bool clrNotification( eEventType event );

};typedef IxEventConsumer *pTIxEventConsumer;

#endif // _IX_EVENT_CONSUMER_H

