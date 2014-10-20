#ifndef _IX_TIMEREVENTCONSUMER
#define _IX_TIMEREVENTCONSUMER

//------------------------------------------------------------------------------
#include "CxVtimer.h"
//------------------------------------------------------------------------------

class IxTimerEventConsumer
{ 
  public:       

    IxTimerEventConsumer(){}
    ~IxTimerEventConsumer(){}
    
    // function's   
    virtual void TimerEventProcessor( unsigned short timerID ) = 0;
    
  protected:  
    
  private:       

    IxTimerEventConsumer( const IxTimerEventConsumer & );
    IxTimerEventConsumer & operator=( const IxTimerEventConsumer & );    
    
}; typedef IxTimerEventConsumer *pIxTimerEventConsumer;

//------------------------------------------------------------------------------

#endif // _IX_TIMEREVENTCONSUMER


