/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

//------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxSysTimer.h"
//------------------------------------------------------------------------------
uint32_t CxSysTimer::timer_sigID = SIGRTMIN;
//------------------------------------------------------------------------------

CxSysTimer::CxSysTimer( const char *name, int64_t period, bool cyclic ):
     expCount( 0 )
   , timerid ( 0 )
{
   memset( &sev, 0, sizeof(sev));
   memset( &its, 0, sizeof(its));
   memset( &sa,  0, sizeof(sa) );

   strncpy( timerName, const_cast<char*>(name), configMAX_TIMER_NAME_LEN );

   // establish handler for timer signal
   sa.sa_flags = SA_SIGINFO | SA_RESTART;
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
   if (true == cyclic)
   {
      its.it_interval.tv_sec = its.it_value.tv_sec;
      its.it_interval.tv_nsec = its.it_value.tv_nsec;
   }

   printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx", __FUNCTION__, timerName, (long) timerid);
}

void CxSysTimer::StartTimer()
{
   if (timerid != 0)
   {
      if (timer_settime(timerid, 0, &its, NULL) == -1)
      {
         printError("CxSysTimer/%s: timer=%s timer_settime error=%d", __FUNCTION__, timerName, errno);
      }
      else
      {
         //printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx Started", __FUNCTION__, timerName, (long) timerid);
      }
   }
   else
   {
      printError("CxSysTimer/%s: timer=%s timerid = 0 !", __FUNCTION__, timerName);
   }
}
void CxSysTimer::StopTimer()
{
   struct itimerspec its_stop;
   // set Stop values ( 0 )
   its_stop.it_value.tv_sec     = 0;
   its_stop.it_value.tv_nsec    = 0;
   its_stop.it_interval.tv_sec  = 0;
   its_stop.it_interval.tv_nsec = 0;

   if (timerid != 0)
   {
      if (timer_settime(timerid, 0, &its_stop, NULL) == -1)
      {
         printError("CxSysTimer/%s: timer=%s timer_settime error=%d!!!", __FUNCTION__, timerName, errno);
      }
      else
      {
         //printDebug("CxSysTimer/%s: timer=%s / ID=0x%lx Stopped", __FUNCTION__, timerName, (long) timerid);
      }
   }
   else
   {
      printError("CxSysTimer/%s: timer=%s timerid = 0 !", __FUNCTION__, timerName);
   }
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

void CxSysTimer::commonHandler(int /*sig*/, siginfo_t *si, void */*uc*/)
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
