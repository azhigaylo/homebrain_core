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
	  
	  eEventType getUnusedEvent ( );
      bool setNotification( eEventType event );
      bool clrNotification( eEventType event );

};typedef IxEventConsumer *pTIxEventConsumer;

#endif // _IX_EVENT_CONSUMER_H

