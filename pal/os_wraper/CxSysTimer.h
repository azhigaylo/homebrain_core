#ifndef _CX_SYSTIMER
#define _CX_SYSTIMER

//------------------------------------------------------------------------------
#include <signal.h>
#include <time.h>

//------------------------------------------------------------------------------

#define configMAX_TIMER_NAME_LEN 50
//------------------------------------------------------------------------------

// name   - timer name, for easy debug only
// period - in nanosecs
// cyclic - if it true - timer will generate cyclic event
  
class CxSysTimer         // class of virtual timer's
{  
  public:

    CxSysTimer( const char *name, uint64_t period, bool cyclic );
    ~CxSysTimer();

    void Start();        // start timer
    void Stop();         // stop timer

  protected:

    char timerName[configMAX_TIMER_NAME_LEN];    

  private:

    static uint16_t timer_sigID;

    int64_t expCount;        // expiration counter
    timer_t timerid;         // timer ID
    struct sigevent sev;     // signal structure
    struct itimerspec its;   // time spec structure
    struct sigaction sa;     // sigaction structure

    static void commonHandler(int sig, siginfo_t *si, void *uc);     // start timer

    virtual void sigHandler();

    CxSysTimer( const CxSysTimer & ){}
    CxSysTimer & operator=( const CxSysTimer & ){}
};

#endif
