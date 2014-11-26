/**********************************************************************
*  Project       Harman Car Multimedia System
*  (c) copyright 2012
*  Company       Harman/Becker Automotive Systems GmbH
*                All rights reserved
*  Secrecy Level STRICTLY CONFIDENTIAL
**********************************************************************/

#include <stdarg.h>
#include <inttypes.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

/**
 * define only in main file
 */
 
#define HBSLOG_MAIN
#include "api/sys/driver/qnx/misc/drvdev/src/hbslog.h"

#include "common.h"
#include "adv7481.h"

int32_t optv_mhl = 1;
uint8_t slogSeverity = 5;

void printError(const char * const fmt,...)
{
   va_list vargs;
   char fmt2[ strlen(fmt) + 40 ];

   strcpy( fmt2, "adv7481-mhl:" );
   strcat( fmt2, fmt );

   va_start( vargs, fmt );
   vslogf( _SLOG_SETCODE(_SLOGC_GRAPHICS, 0), _SLOG_DEBUG1, fmt2, vargs );
   va_end( vargs );
}

void printInfo(const char * const fmt, ...)
{
   //if (optv_mhl > 1)
   {
      va_list vargs;
      char fmt2[ strlen(fmt) + 40 ];

      strcpy( fmt2, "adv7481-mhl:" );
      strcat( fmt2, fmt );

      va_start( vargs, fmt );
      vslogf( _SLOG_SETCODE(_SLOGC_GRAPHICS, 0), _SLOG_DEBUG1, fmt2, vargs );
      va_end( vargs );
   }
}

void printWarning(const char * const fmt, ...)
{
   //if (optv_mhl > 0)
   {
      va_list vargs;
      char fmt2[ strlen(fmt) + 40 ];

      strcpy( fmt2, "adv7481-mhl:" );
      strcat( fmt2, fmt );

      va_start( vargs, fmt );
      vslogf( _SLOG_SETCODE(_SLOGC_GRAPHICS, 0), _SLOG_DEBUG1, fmt2, vargs );
      va_end( vargs );
   }
}

void printDebug(const char * const fmt, ...)
{
   //if (optv_mhl > 2)
   {
      va_list vargs;
      char fmt2[ strlen(fmt) + 40 ];

      strcpy( fmt2, "adv7481-mhl:" );
      strcat( fmt2, fmt );

      va_start( vargs, fmt );
      vslogf( _SLOG_SETCODE(_SLOGC_GRAPHICS, 0), _SLOG_DEBUG1, fmt2, vargs );
      va_end( vargs );
   }
}
