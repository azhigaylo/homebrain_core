#ifndef _IX_EVENT_CONSUMER
#define _IX_EVENT_CONSUMER

#include "CxEvent.h"

//------------------------------------------------------------------------------
//----------------interface class for evrnt consumer objects--------------------
//------------------------------------------------------------------------------
 
class IxEventConsumer  
{
 public:
   IxEventConsumer( ); 
   virtual ~IxEventConsumer( );
   virtual bool processEvent( pTEvent pEvent ) = 0;   
   bool setNotification( CxEvent::EventType );
   bool clrNotification( CxEvent::EventType );
};typedef IxEventConsumer *pTIxEventConsumer; // end of IxEventConsumer

#endif // _IX_EVENT_CONSUMER_H

