/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_EVENT_PROCESSOR
#define _CX_EVENT_PROCESSOR

//------------------------------------------------------------------------------
#include <vector>

#include "common/ptypes.h"
#include "os_wrapper/CxQueue.h"
#include "os_wrapper/IxRunnable.h"
#include "os_wrapper/CxMutexLocker.h"

#include "CxEvent.h"
#include "IxEventConsumer.h"
#include "IxEventProducer.h"
//------------------------------------------------------------------------------
#define configEVENT_POOL_SIZE         (100)      // in event

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

    bool setEvent( TEvent Event );
    TEvent getEvent( );

    friend bool IxEventConsumer::setNotification( eEventType event );
    friend bool IxEventConsumer::clrNotification( eEventType event );
    friend bool IxEventProducer::sendEvent( eEventType event, uint16_t containerSize, void* );

 private:

    std::vector<TNotificationUnit> NotificationPool;
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
