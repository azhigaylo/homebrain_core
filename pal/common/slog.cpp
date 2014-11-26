//------------------------------------------------------------------------------
#include <stdarg.h>
#include <inttypes.h>
#include <syslog.h>
//------------------------------------------------------------------------------ 

int32_t optv = 5;

void printError(const char * const fmt,...)
{
   va_list vargs;
   setlogmask (LOG_UPTO (LOG_ERR));
   openlog ("home-brain:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

   va_start( vargs, fmt );
   vsyslog( LOG_ERR, fmt, vargs );
   va_end( vargs );

   closelog ();
}

void printWarning(const char * const fmt, ...)
{
   if (optv > 0)
   {
      va_list vargs;
      setlogmask (LOG_UPTO (LOG_WARNING));
      openlog ("home-brain:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

      va_start( vargs, fmt );
      vsyslog( LOG_WARNING, fmt, vargs );
      va_end( vargs );

      closelog ();
   }
}

void printDebug(const char * const fmt, ...)
{
   if (optv > 1)
   {
      va_list vargs;
      setlogmask (LOG_UPTO (LOG_NOTICE));
      openlog ("home-brain:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

      va_start( vargs, fmt );
      vsyslog( LOG_NOTICE, fmt, vargs );
      va_end( vargs );

      closelog ();
   }
}
