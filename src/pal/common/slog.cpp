//------------------------------------------------------------------------------
#include <stdarg.h>
#include <inttypes.h>
#include <syslog.h>
#include <cstdio>
#include <string.h>
#include <iostream>

//------------------------------------------------------------------------------ 
using namespace std;
//------------------------------------------------------------------------------ 

int32_t optv = 0;

void setDbgLevel(int lvl)
{
   optv = lvl;
}

void printError(const char * const fmt,...)
{
   va_list vargs;

   if (optv == 4)
   {
      static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
      pthread_mutex_lock( &cs_mutex );
       char Data[2000];
       memset(&Data[0],0,sizeof(Data));

       va_start( vargs, fmt );
       vsnprintf(Data,((sizeof(Data)) - 1), fmt, vargs);
       va_end( vargs );
       printf("%s \n",Data);
       pthread_mutex_unlock( &cs_mutex );   
   }
   else
   {
      setlogmask (LOG_UPTO (LOG_ERR));

      va_start( vargs, fmt );
      vsyslog( LOG_ERR, fmt, vargs );
      va_end( vargs );
   }
}

void printWarning(const char * const fmt, ...)
{
   va_list vargs;
      
   if (optv == 4)
   {
      static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
      pthread_mutex_lock( &cs_mutex );
       char Data[2000];
       memset(&Data[0],0,sizeof(Data));

       va_start( vargs, fmt );
       vsnprintf(Data,((sizeof(Data)) - 1), fmt, vargs);
       va_end( vargs );
       printf("%s\n",Data);
       pthread_mutex_unlock( &cs_mutex );   
   }
   else   
   {
      if (optv > 0)
      {
         setlogmask (LOG_UPTO (LOG_ERR));

         va_start( vargs, fmt );
         vsyslog( LOG_ERR, fmt, vargs );

         va_end( vargs );
      }
   }
}

void printDebug(const char * const fmt, ...)
{
   va_list vargs;
      
   if (optv == 4)
   {
      static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
      pthread_mutex_lock( &cs_mutex );
       char Data[2000];
       memset(&Data[0],0,sizeof(Data));

       va_start( vargs, fmt );
       vsnprintf(Data,((sizeof(Data)) - 1), fmt, vargs);
       va_end( vargs );
       printf("%s\n",Data);
       pthread_mutex_unlock( &cs_mutex );   
   }
   else   
   {
      if (optv > 1)
      {
         setlogmask (LOG_UPTO (LOG_ERR));
         va_start( vargs, fmt );
          vsyslog( LOG_ERR, fmt, vargs );
        va_end( vargs );
      }
   }
}
