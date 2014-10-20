#ifndef _CX_TST
#define _CX_TST

//------------------------------------------------------------------------------
#include "CxThreadIO.h"
#include "..\vtimer\CxVTimer.h"
#include "..\vtimer\IxTimerEventConsumer.h"
//------------------------------------------------------------------------------

/*

serve for:
 1. configuration of ports / units
 2. start system / debugging / devices threads
 3. put thread in sleep

*/

class CxTstThread : public CxThreadIO, public IxTimerEventConsumer
{   
  enum TLauncherState
  {
      ST_L_UNKNOWN = 0,
      ST_L_CONFIG,
      ST_L_WAIT_DRV_RESP,
      ST_L_NORMAL_WORK,
      ST_L_SLEEP
  };
  
  public:  

     // function's   
     CxTstThread();       
     ~CxTstThread();
     
     static CxTstThread& getInstance();
     
     // start all parts of system task
     void Start();

     void SysTimerChecker();
     
  protected:    
    
     virtual void TimerEventProcessor( unsigned short timerID );
    
     virtual void ThreadProcessor();
     virtual void CommandProcessor( TCommand &Command ){}

     void WorkCycle();
     
     CxVTimer IntrvalTimer;          
    
  private:       
     // FSM process
     void Process();
     
     bool flash_y;
     bool flash_g;
     bool flash_on;
     
     TLauncherState LauncherState;
     
     int fileID;
     
     long tick_counter;

 }; typedef CxTstThread *pCxTstThread;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

