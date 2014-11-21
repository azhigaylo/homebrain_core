#ifndef _CX_SYSTIMER
#define _CX_SYSTIMER


//------------------------------------------------------------------------------
// DO NOT USE FOR FUNCTION WHICH NEEDS LONG PROCESSOR TIME
// vApplicationTickHook SECTION IS COLLED FROM OS KERNEL 
// TIME CRITICAL 
// ONLY FOR INTERVAL TIMER
//------------------------------------------------------------------------------


//----------------------only wraper for FREERTOS xQueue-------------------------

#include "FreeRTOS.h"
#include "task.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
   void vApplicationTickHook( void );
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
//---------------------class for system virtual timer's-------------------------
//------------------------------------------------------------------------------

class CxSysVirtualTimer;
typedef class CxSysVirtualTimer *pCxSysVirtualTimer;

struct TListOfVirtualTimers
{                                                                               // sructure for list 
    pCxSysVirtualTimer  FirstElem;
    pCxSysVirtualTimer  LastElem;
    unsigned char  CounterTimer;
}; 
    
//------------------------------------------------------------------------------

class CxSysVirtualTimer                                                                  // class of virtual timer's
{  
  public:                    
    pCxSysVirtualTimer    pNextTmr;                                                       // pointer on next virtual timer
    
    enum TMode
    {
      cycle = 1,
      singl
    };
    enum TState
    {
      on = 1,
      off
    };                                                        // timer on/off  in start time  

    CxSysVirtualTimer(void (*execution_func)(), unsigned long period, TMode mode, TState state);
    ~CxSysVirtualTimer();
          
    void  Start();                                         // start timer    
    void  Stop()   {mCount = 0; mWork = false;}            // stop timer
    void  Cycle()  {mCycle = cycle;}                       // timer in cicle
    void  NoCycle(){mCycle = singl;}                       // out fro cikl work timer's
    bool  Work()   {return(mWork);}                        // while timer count - true  
    void  Dec();                                           // check on  
    
  private:               
    
    void AddToList();                                      // add virtual timer in work list
    void Over();                                           // function of overflow of virtual timer's                        
 
    void (*exec_func)();                                   // execution function 
    unsigned long         mCount;                          // work period
    TMode                 mCycle;                          // cyclic flag 

  public:  
    bool                  mWork;                           // work flag 
    unsigned long         mCountConst;                     // constant timer period
};

//------------------------------------------------------------------------------

class CxTSysBase
{
  public:
    
    static CxTSysBase& getInstance();    

    void SysTic();
    
  protected:
    
    CxTSysBase();
    ~CxTSysBase(){}

}; typedef CxTSysBase *pCxTSysBase;

#endif
