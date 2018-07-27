//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "eventpool/CxEventDispatcher.h"
//------------------------------------------------------------------------------
using namespace event_pool;
//------------------------------------------------------------------------------
CxEventDispatcher* CxEventDispatcher::theInstance = 0;
CxMutex CxEventDispatcher::singlEventLock("singlEventLocker");
//------------------------------------------------------------------------------

CxEventDispatcher::CxEventDispatcher():
    IxRunnable ( "EVENT_TASK" )
   ,NotificationPool( )
   ,EventPool( "event_queue", configEVENT_POOL_SIZE, sizeof(TEvent), true )
   ,lastFreeEventId( event_pool::EVENT_LAST_EVENT )
{
   task_run( );
}

CxEventDispatcher::~CxEventDispatcher( )
{
   task_stop();
}

//------------------------------------------------------------------------------

CxEventDispatcher * CxEventDispatcher::getInstance( )
{
   if(CxEventDispatcher::theInstance == 0)
   {
      CxMutexLocker locker(&CxEventDispatcher::singlEventLock);

      if(CxEventDispatcher::theInstance == 0)
      {
          CxEventDispatcher::theInstance = new CxEventDispatcher();
      }
   }

  return CxEventDispatcher::theInstance;
}

void CxEventDispatcher::delInstance()
{
   if(CxEventDispatcher::theInstance != 0)
   {
      delete CxEventDispatcher::theInstance;
      printDebug("CxEventDispatcher/%s: instance deleted", __FUNCTION__);
   }
}

//------------------------------------------------------------------------------

bool CxEventDispatcher::NotificationPoolIsEmpty()
{
   if( NotificationPool.size() > 0 ) return false;
    else return true;;
}

unsigned short CxEventDispatcher::GetNotificationRecNumb()const
{
   return( NotificationPool.size() );
}

bool CxEventDispatcher::setEvent( TEvent Event )
{
   return EventPool.send( &Event, sizeof(TEvent) );
}

TEvent CxEventDispatcher::getEvent()
{
   TEvent dummy_event = { event_pool::EVENT_DUMMY, 0, NULL };
   TEvent       event = { event_pool::EVENT_DUMMY, 0, NULL };

   if (-1 != EventPool.receive( &event, sizeof(TEvent) ))
   {
       return event;
   }
   return dummy_event;
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

   if( (NotificationPool.size() > 0) && (curentEvent.eventType != event_pool::EVENT_DUMMY) )
   {
      TNotificationUnit NotificationUnit;

      for( unsigned short element = 0; element < NotificationPool.size(); element++ )
      {
        NotificationUnit = NotificationPool[element];
        if( NotificationUnit.event.eventType == curentEvent.eventType )
        {
          NotificationUnit.pIxEventConsumer->processEvent( &curentEvent );
        }
      }
    }
    sleep_mcs(50);
}

bool CxEventDispatcher::setNotification( TEvent event, pTIxEventConsumer pIxEventConsumer )
{
   TNotificationUnit NotificationUnit = { event, pIxEventConsumer };

   CxMutexLocker locker(&CxEventDispatcher::singlEventLock);
   NotificationPool.push_back( NotificationUnit );
   printDebug("CxEventDispatcher/%s: subscribe on event = %i ", __FUNCTION__, event.eventType);
   return true;
}

bool CxEventDispatcher::clrNotification(TEvent event, pTIxEventConsumer pIxEventConsumer)
{
   for( unsigned short element = 0; element < NotificationPool.size(); element++ )
   {
      TNotificationUnit notification_unit = NotificationPool[element];

      if( notification_unit.event.eventType == event.eventType &&
          notification_unit.pIxEventConsumer == pIxEventConsumer )
      {
          NotificationPool.erase(NotificationPool.begin() + element);
          return true;
      }
  }
   return false;
}
