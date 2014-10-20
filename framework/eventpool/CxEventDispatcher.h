#ifndef _CX_EVENT_PROCESSOR
#define _CX_EVENT_PROCESSOR

#include "CxQueue.h"
#include "IxRunnable.h"
#include "CxEvent.h"
#include "IxEventConsumer.h"
#include "IxEventProducer.h"
#include "..\staticpool\CxVector.h"

#define configEVENT_POOL_SIZE         (50)      // in event
#define configNOTIFICATION_POOL_SIZE  (50)      // in notification unit

//------------------------------------------------------------------------------ 
//-------------------------------BASE UART class-------------------------------- 

void EVENT_DISPATCH();
void EVENT_GEN( CxEvent::EventType event, void *eventContainer );

//------------------------------------------------------------------------------ 
#pragma pack ( 1 )
struct TNotificationUnit
{
   TEvent event;
   pTIxEventConsumer pIxEventConsumer;
};
#pragma pack ( )

typedef TNotificationUnit *pTNotificationUnit;

class CxEventPool
{
 public:
   CxEventPool( );
   ~CxEventPool( );
   
   bool isEmpty()const;
   bool setEvent( TEvent Event );
   TEvent getEvent( );
 private:
   CxQueue EventPool; 
   char  cEventRecNumb;          // number of element in event pool
};

class CxEventDispatcher : public IxRunnable
{
 public: 
   
    static CxEventDispatcher& getInstance( );    

    bool NotificationPoolIsEmpty( );
    unsigned short GetNotificationRecNumb( )const;
    
    friend bool IxEventConsumer::setNotification( CxEvent::EventType  );
    friend bool IxEventConsumer::clrNotification( CxEvent::EventType  ); 
    friend bool IxEventProducer::sendEvent( CxEvent::EventType, void* );
    
    void start( );
        
 private:   

    CxEventDispatcher();
    
    CxEventPool EventPool;
    CxVector<TNotificationUnit> NotificationPool;

    bool setNotification( TEvent event, pTIxEventConsumer pIxEventConsumer );
    bool clrNotification( TEvent event, pTIxEventConsumer pIxEventConsumer );    
    void Dispatch();
        
    virtual void TaskProcessor( );

    ~CxEventDispatcher( ){}

    unsigned short sNotificationUnitRecNumb;  // number of element in notification pool  

}; 
typedef CxEventDispatcher *pCxEventDispatcher;

#endif /*_CX_EVENT_PROCESSOR*/   
