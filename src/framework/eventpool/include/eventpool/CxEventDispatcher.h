#ifndef _CX_EVENT_PROCESSOR
#define _CX_EVENT_PROCESSOR

//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "staticpool/CxVector.h"
#include "os_wrapper/CxQueue.h"
#include "os_wrapper/IxRunnable.h"
#include "CxEvent.h"
#include "IxEventConsumer.h"
#include "IxEventProducer.h"
//------------------------------------------------------------------------------
#define configEVENT_POOL_SIZE         (10)      // in event
#define configNOTIFICATION_POOL_SIZE  (50)      // in notification unit
//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TNotificationUnit
{
   TEvent event;
   pTIxEventConsumer pIxEventConsumer;
};
#pragma pack ( )

typedef TNotificationUnit *pTNotificationUnit;

class CxEventDispatcher : public IxRunnable
{
 public:

    static CxEventDispatcher * getInstance( );
    static void delInstance();

    bool NotificationPoolIsEmpty( );
    unsigned short GetNotificationRecNumb( )const;

    eEventType getVirtualEvent();
    bool setEvent( TEvent Event );
    TEvent getEvent( );

    friend bool IxEventConsumer::setNotification( eEventType event );
    friend bool IxEventConsumer::clrNotification( eEventType event );
    friend bool IxEventProducer::sendEvent( eEventType event, uint16_t containerSize, void* );

 private:

    CxVector<TNotificationUnit> NotificationPool;
    unsigned short sNotificationUnitRecNumb;  // number of element in notification pool
    CxQueue EventPool;
    eEventType lastFreeEventId;

    bool setNotification( TEvent event, pTIxEventConsumer pIxEventConsumer );
    bool clrNotification( TEvent event, pTIxEventConsumer pIxEventConsumer );
    void Dispatch();

    virtual void TaskProcessor( );

	static CxMutex singlEventLock;
	static CxEventDispatcher* theInstance;

    CxEventDispatcher();
    virtual ~CxEventDispatcher( );

};
typedef CxEventDispatcher *pTCxEventDispatcher;

#endif /*_CX_EVENT_PROCESSOR*/
