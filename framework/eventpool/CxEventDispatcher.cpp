//------------------------------------------------------------------------------

#include "CxEventDispatcher.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxEventPool::CxEventPool():
  cEventRecNumb( 0 ),
  EventPool( configEVENT_POOL_SIZE, sizeof(TEvent) )
{

}

CxEventPool::~CxEventPool()
{

}

bool CxEventPool::isEmpty()const
{
   if( cEventRecNumb != 0 )return false;
     else return true;
}

bool CxEventPool::setEvent( TEvent Event )
{
  bool result = false; 
  // work with pool
  if( cEventRecNumb < configEVENT_POOL_SIZE )
  {
    if( true == EventPool.Send( &Event ) ) 
    {  
      cEventRecNumb++;    
      result = true; 
    }  
  }
  
  return result;  
}

TEvent CxEventPool::getEvent()
{
  TEvent Event = { CxEvent::EVENT_DUMMY, NULL }; 
  
  if( cEventRecNumb > 0 )
  {
    // work with pool    
    if( true == EventPool.Receive( &Event ) )
    {  
      cEventRecNumb--;      
    }    
  }
    
  return Event;
}

//------------------------------------------------------------------------------ 

CxEventDispatcher::CxEventDispatcher():
   IxRunnable ( "EVENT_TASK" )
  ,sNotificationUnitRecNumb( 0 )
  ,NotificationPool( configNOTIFICATION_POOL_SIZE )
{    

}

CxEventDispatcher &CxEventDispatcher::getInstance()
{
  static CxEventDispatcher theInstance;
  return theInstance;
}

void CxEventDispatcher::TaskProcessor()
{
  Dispatch();
}
 
void CxEventDispatcher::Dispatch()
{
  while( EventPool.isEmpty() == false )
  {
    TEvent curentEvent = EventPool.getEvent();  
    
    if( (sNotificationUnitRecNumb > 0) && (curentEvent.eventType != CxEvent::EVENT_DUMMY) )
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
}

bool CxEventDispatcher::setNotification( TEvent event, pTIxEventConsumer pIxEventConsumer )
{
  TNotificationUnit NotificationUnit = { event, pIxEventConsumer };

  if( sNotificationUnitRecNumb < configNOTIFICATION_POOL_SIZE )
  {
    // work with pool       
    NotificationPool.add( NotificationUnit );
    sNotificationUnitRecNumb++;    
    return true;      
  }  
  return false;
}

bool CxEventDispatcher::clrNotification(TEvent event, pTIxEventConsumer pIxEventConsumer)
{
  return false;
}

bool CxEventDispatcher::NotificationPoolIsEmpty()
{
   if( sNotificationUnitRecNumb > 0 ) return false;
    else return true;;
}

unsigned short CxEventDispatcher::GetNotificationRecNumb()const
{
  return( sNotificationUnitRecNumb );
}

void CxEventDispatcher::start( )
{
  DBG_SCOPE( CxEventDispatcher, CxEventDispatcher )
  
  DBG_MSG( ("[M] Event Dispatcher has been runned\n\r") );  
  
  task_run( );
}  
