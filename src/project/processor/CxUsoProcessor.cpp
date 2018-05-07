//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "common/slog.h"
#include "common/utils.h"
#include "processor/CxUsoProcessor.h"
#include "uso/USODefinition.h"
//------------------------------------------------------------------------------

CxUsoProcessor::CxUsoProcessor( const char * sPrcName, const char *sInterfaceNmae ) :
    IxRunnable     ( sPrcName )
   ,sUsedInterface ( 0 )
   ,logDevCounter  ( 0 )
   ,LOGDEV_LIST    ( logDevItemsSize )
{
   sUsedInterface = strdup(sInterfaceNmae);

   printDebug("CxUsoProcessor/%s: Processor for %s created ", __FUNCTION__, sUsedInterface );
}

CxUsoProcessor::~CxUsoProcessor( )
{
   printDebug("CxUsoProcessor/%s: Processor for %s removed ", __FUNCTION__, sUsedInterface );

   if (0 != sUsedInterface)
   {
      free( sUsedInterface );
   }

   // delete my own task
   task_stop( );
}

//------------------------------------------------------------------------------

bool CxUsoProcessor::set_logdev( IxLogDevice *pLogDev )
{
   bool result = false;

   // add pointer on logdev in vector
   TLogDevListItem logDevListItemTmp = {static_cast<int8_t>(logDevCounter), pLogDev};

   result = LOGDEV_LIST.add( logDevListItemTmp );

   // counter increment
   if( true == result )
   {
      logDevCounter++;
      printDebug("CxUsoProcessor/%s: LogDev = %s add OK ", __FUNCTION__, pLogDev->getDeviceName());
   }
   else
   {
      printError("CxUsoProcessor/%s: LogDev = %s add error ", __FUNCTION__, pLogDev->getDeviceName());
   }

   return result;
}

char *CxUsoProcessor::get_interfacename( )const
{
   return sUsedInterface;
}

//------------------------------------------------------------------------------

void CxUsoProcessor::TaskProcessor()
{
   if( logDevCounter == LOGDEV_LIST.count() )
   {
      uint16_t errCounter = 0;

      for( uint8_t itr = 0; itr < logDevCounter; itr++ )
      {
         IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;

         if( NULL != pDevice )
         {
            if (false == pDevice->Process())
            {
               errCounter++;
            }
         }
      }

      // there was situation where CPU was loaded by this process
      // because in situation when we have communication error with
      // all uso modules on this port cpu was not locked on the communication task
      if (errCounter == LOGDEV_LIST.count())
      {
         sleep_mcs(200000);
      }

   }

   sleep_mcs(50);
}
