//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutexLocker.h"
#include "CxEventDispatcher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------ 
CxEventDispatcher* CxEventDispatcher::theInstance = 0;
CxMutex CxEventDispatcher::singlEventLock("singlEventLocker");
//------------------------------------------------------------------------------ 

CxEventDispatcher::CxEventDispatcher():
    IxRunnable ( "EVENT_TASK" )
   ,NotificationPool( configNOTIFICATION_POOL_SIZE )
   ,sNotificationUnitRecNumb( 0 )
   ,EventPool( "event_queue", configEVENT_POOL_SIZE, sizeof(TEvent), true )
   ,lastFreeEventId( event_pool::EVENT_LAST_EVENT )
{    
   task_run( );
}

//------------------------------------------------------------------------------

CxEventDispatcher * CxEventDispatcher::getInstance( )
{
   if(theInstance == 0)
   {
      CxMutexLocker locker(&CxEventDispatcher::singlEventLock);

      if(theInstance == 0)
      {
         theInstance = new CxEventDispatcher();
      }
   }

  return theInstance;
}

void CxEventDispatcher::delInstance()
{
   if(theInstance != 0)
   {
      delete this;
   }
}

//------------------------------------------------------------------------------

bool CxEventDispatcher::NotificationPoolIsEmpty()
{
   if( sNotificationUnitRecNumb > 0 ) return false;
    else return true;;
}

unsigned short CxEventDispatcher::GetNotificationRecNumb()const
{
   return( sNotificationUnitRecNumb );
}

eEventType CxEventDispatcher::getVirtualEvent()
{
   CxMutexLocker locker(&CxEventDispatcher::singlEventLock);

   // hot fix
   lastFreeEventId = (eEventType)((static_cast<int>(lastFreeEventId)) + 1);

   return lastFreeEventId;
}

bool CxEventDispatcher::setEvent( TEvent Event )
{
   return EventPool.send( &Event, sizeof(TEvent) );
}

TEvent CxEventDispatcher::getEvent()
{
   TEvent Event = { event_pool::EVENT_DUMMY, 0, NULL };

   EventPool.receive( &Event, sizeof(TEvent) );

   return Event;
}

//------------------------------------------------------------------------------

void CxEventDispatcher::TaskProcessor()
{
   Dispatch();
}
 
void CxEventDispatcher::Dispatch()
{
   // blocked call
   TEvent curentEvent = getEvent();
   // blocked call

   CxMutexLocker locker(&CxEventDispatcher::singlEventLock);

   if( (sNotificationUnitRecNumb > 0) && (curentEvent.eventType != event_pool::EVENT_DUMMY) )
   {
      TNotificationUnit NotificationUnit;

      for( unsigned short element = 0; element < sNotificationUnitRecNumb; element++ )
      {
        NotificationUnit = NotificationPool[element];
        if( NotificationUnit.event.eventType == curentEvent.eventType )
        {
          NotificationUnit.pIxEventConsumer->processEvent( &curentEvent );
        }
      }
    }
}

bool CxEventDispatcher::setNotification( TEvent event, pTIxEventConsumer pIxEventConsumer )
{
   TNotificationUnit NotificationUnit = { event, pIxEventConsumer };

   CxMutexLocker locker(&CxEventDispatcher::singlEventLock);

   if( sNotificationUnitRecNumb < configNOTIFICATION_POOL_SIZE )
   {
     // work with pool       
     NotificationPool.add( NotificationUnit );
     sNotificationUnitRecNumb++;

     printDebug("CxEventDispatcher/%s: subscribe on event = %i ", __FUNCTION__, event.eventType);	 
     return true;      
   }  
   return false;
}

bool CxEventDispatcher::clrNotification(TEvent event, pTIxEventConsumer pIxEventConsumer)
{
   return false;
}
