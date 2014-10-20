#ifndef _VTIMER_CLASS
#define _VTIMER_CLASS

#include "IxRunnable.h"
#include "IxTimerEventConsumer.h"

//------------------------------------------------------------------------------
//---------------------class for virtual timer's--------------------------------
//------------------------------------------------------------------------------

class CxVTimer;
typedef class CxVTimer *pCxVTimer;

struct TListTimer{                                                              // sructure for list 
    pCxVTimer     FirstElem;
    pCxVTimer     LastElem;
    unsigned char  CounterTimer;
}; 
    
//------------------------------------------------------------------------------

class CxVTimer                                                                  // class of virtual timer's
{  
  public:                    
    pCxVTimer    NextTmr;                                                       // pointer on next virtual timer
    
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

    CxVTimer(unsigned long period, TMode mode, TState state);    
    CxVTimer(void (*execution_func)(), unsigned long period, TMode mode, TState state);
    CxVTimer( pIxTimerEventConsumer consumer, unsigned long period, TMode mode, TState state);
    ~CxVTimer();
          
    void  Start();                                         // start timer    
    void  Stop()   {mCount = 0; mWork = false;}            // stop timer
    void  Cycle()  {mCycle = cycle;}                       // timer in cicle
    void  NoCycle(){mCycle = singl;}                       // out fro cikl work timer's
    bool  Work()   {return(mWork);}                        // while timer count - true  
    unsigned short GetTimerID();                           // timer ID return 
    void  TimerCheck();                                    // check on  
            
  private:               
    
    void AddToList();                                      // add virtual timer in work list
    void Over();                                           // function of overflow of virtual timer's                        
 
    void (*exec_func)();                                   // execution function 
    unsigned long         mCount;                          // work period
    TMode                 mCycle;                          // cyclic flag 
    
    unsigned short timerID;
    
    pIxTimerEventConsumer pTimerEventConsumer;
      
  public:  
    bool                  mWork;                           // work flag 
    unsigned long         mCountConst;                     // constant timer period
};

class CxTBase : public IxRunnable
{
  public:
    
    static CxTBase& getInstance();    

    virtual void TaskProcessor();

  protected:
    
    CxTBase();
    ~CxTBase(){}
    
    void SysTic(); 

}; typedef CxTBase *pCxTBase;


#endif /*_VTIMER_CLASS*/
