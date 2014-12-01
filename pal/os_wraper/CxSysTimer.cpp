//------------------------------------------------------------------------------
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxSysTimer.h"
//------------------------------------------------------------------------------
uint16_t CxSysTimer::timer_sigID = SIGRTMIN;
//------------------------------------------------------------------------------

CxSysTimer::CxSysTimer( const char *name, uint64_t period, bool cyclic ): 
    timerid ( 0 )
   ,expCount( 0 )
{
   memset_m( &sev, 0, sizeof(sev), sizeof(sev) );
   memset_m( &its, 0, sizeof(its), sizeof(its) );
   memset_m( &sa,  0, sizeof(sa),  sizeof(sa)  );
   
   strncpy_m( timerName, const_cast<char*>(name), configMAX_TIMER_NAME_LEN );
   
   // establish handler for timer signal
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = CxSysTimer::commonHandler;
   sigemptyset(&sa.sa_mask);
   if (sigaction(timer_sigID, &sa, NULL) == -1)
   {
      printError("CxSysTimer/%s: timer=%s sigaction error!!!", __FUNCTION__, timerName);
   }

   // create the timer
   sev.sigev_notify = SIGEV_SIGNAL;
   sev.sigev_signo = timer_sigID;
   sev.sigev_value.sival_ptr = (void*) this;
   if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)
   {
      printError("CxSysTimer/%s: timer=%s timer_create error!!!", __FUNCTION__, timerName);
   }
   
   // init the timer
   its.it_value.tv_sec = period / 1000000000;
   its.it_value.tv_nsec = period % 1000000000;
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;   
   
   printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx", __FUNCTION__, timerName, (long) timerid);
}

void CxSysTimer::Start()
{
   if (timerid != 0)
   {
      if (timer_settime(timerid, 0, &its, NULL) == -1)
      {
         printError("CxSysTimer/%s: timer=%s timer_settime error!!!", __FUNCTION__, timerName);
      }
   }
   else
   {
      printError("CxSysTimer/%s: timer=%s timerid = 0 !", __FUNCTION__, timerName);
   }
} 
void CxSysTimer::Stop()
{
   struct itimerspec its_stop;
   // set Stop values ( 0 )
   its_stop.it_value.tv_sec  = 0;
   its_stop.it_value.tv_nsec = 0;

   if (timerid != 0)
   {
      if (timer_settime(timerid, 0, &its, NULL) == -1)
      {
         printError("CxSysTimer/%s: timer=%s timer_settime error!!!", __FUNCTION__, timerName);
      }
   }
   else
   {
      printError("CxSysTimer/%s: timer=%s timerid = 0 !", __FUNCTION__, timerName);
   }
   
   printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx Stopped", __FUNCTION__, timerName, (long) timerid);
}

CxSysTimer::~CxSysTimer()
{
   if (timerid != 0)
   {
      timer_delete( timerid );
	  printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx Deleted", __FUNCTION__, timerName, (long) timerid);
   }
}

//------------------------------------------------------------------------------

void CxSysTimer::commonHandler(int sig, siginfo_t *si, void *uc)
{   
   CxSysTimer *pSysTimer = (reinterpret_cast<CxSysTimer*>(si->si_value.sival_ptr)) ;

   if (pSysTimer != 0)
   {
      pSysTimer->sigHandler();
	  pSysTimer->expCount++;
   }
}

void CxSysTimer::sigHandler()
{
   printWarning("CxSysTimer/%s: timer=%s expired n=%i times, handler not implemented!", __FUNCTION__, timerName, expCount);
}


//------------------------------------------------------------------------------
