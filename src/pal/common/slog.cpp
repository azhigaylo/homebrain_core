/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

//------------------------------------------------------------------------------
#include <stdarg.h>
#include <inttypes.h>
#include <syslog.h>
#include <cstdio>
#include <string.h>
#include <iostream>

#include "common/slog.h"
#include "dlt/dlt.h"

//-------------------------------------------------------------------------------
using namespace std;
//-------------------------------------------------------------------------------

static volatile int32_t optv = MSG_ERROR;
static volatile int32_t dbg_sink = TRACE_SINK_CONSOLE;

DLT_DECLARE_CONTEXT(dlt_context);

pthread_mutex_t cs_mutex;

//---------------------------------mutex init------------------------------------
void printMutexInit()
{
   pthread_mutexattr_t mutexattr;

   pthread_mutexattr_init(&mutexattr);
   pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);

   if (0 != pthread_mutex_init(&cs_mutex,&mutexattr))
   {
      printf("print mutex init error !!!");
   }
}

//---------------------------------CONSOLE---------------------------------------

void printConsole(const char *msg)
{
    if (0 != pthread_mutex_lock(&cs_mutex)) printf("print console mutex lock error !!!");

    printf("%s \n", msg);

    if (0 != pthread_mutex_unlock(&cs_mutex)) printf("print console mutex unlock error !!!");
}

//---------------------------------DLT-------------------------------------------

void printErrorDlt(const char *msg)
{
    DLT_LOG(dlt_context, DLT_LOG_ERROR, DLT_STRING(msg));
}

void printWarningDlt(const char *msg)
{

    DLT_LOG(dlt_context, DLT_LOG_WARN, DLT_STRING(msg));
}

void printInfoDlt(const char *msg)
{
    DLT_LOG(dlt_context, DLT_LOG_INFO, DLT_STRING(msg));
}

void printDebugDlt(const char *msg)
{
    DLT_LOG(dlt_context, DLT_LOG_DEBUG, DLT_STRING(msg));
}

//------------------------------------------------------------------------------

void setDbgLevel(int lvl)
{
   optv = lvl;
   printMutexInit();
}

void setDbgSink(int sink)
{
   dbg_sink = sink;
}

int getDbgSink()
{
   return dbg_sink;
}

void initDlt()
{
   DLT_REGISTER_APP("HB","homebrain trace");
   DLT_REGISTER_CONTEXT(dlt_context,"HB","core app");

   usleep(10000);
}

void deinitDlt()
{
    DLT_UNREGISTER_CONTEXT(dlt_context);

    DLT_UNREGISTER_APP();
}

void printError(const char * const fmt,...)
{
   // get string from va list
   va_list vargs;
   char data[1000];

   va_start( vargs, fmt );
   vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
   va_end( vargs );

   // send to sink
   switch (dbg_sink)
   {
      case TRACE_SINK_DLT :
      {
         printErrorDlt( data );
         break;
      }
      case TRACE_SINK_CONSOLE :
      default:
      {
         printConsole( data );
         break;
      }
   }
}

void printWarning(const char * const fmt, ...)
{
   if (optv > 0)
   {
       // get string from va list
       va_list vargs;
       char data[1000];

       va_start( vargs, fmt );
       vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
       va_end( vargs );

       // send to sink
       switch (dbg_sink)
       {
          case TRACE_SINK_DLT :
          {
             printWarningDlt( data );
             break;
          }
          case TRACE_SINK_CONSOLE :
          default:
          {
             printConsole( data );
             break;
          }
       }
   }
}

void printInfo(const char * const fmt, ...)
{
   if (optv > 1)
   {
       // get string from va list
       va_list vargs;
       char data[1000];

       va_start( vargs, fmt );
       vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
       va_end( vargs );
       // send to sink
       switch (dbg_sink)
       {
          case TRACE_SINK_DLT :
          {
             printInfoDlt( data );
             break;
          }
          case TRACE_SINK_CONSOLE :
          default:
          {
             printConsole( data );
             break;
          }
       }
   }
}

void printDebug(const char * const fmt, ...)
{
   if (optv > 2)
   {
       // get string from va list
       va_list vargs;
       char data[1000];

       va_start( vargs, fmt );
       vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
       va_end( vargs );
       // send to sink
       switch (dbg_sink)
       {
          case TRACE_SINK_DLT :
          {
             printDebugDlt( data );
             break;
          }
          case TRACE_SINK_CONSOLE :
          default:
          {
             printConsole( data );
             break;
          }
       }
   }
}

