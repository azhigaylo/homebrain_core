//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "slog.h"
#include "utils.h"

#include "CxUsoProcessor.h"
//------------------------------------------------------------------------------

CxUsoProcessor::CxUsoProcessor( const char *sInterfaceNmae ) :
    IxRunnable     ( sInterfaceNmae )
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
}

//------------------------------------------------------------------------------

bool CxUsoProcessor::set_logdev( IxLogDevice *pLogDev )
{
   bool result = false;

   // add pointer on logdev in vector
   TLogDevListItem logDevListItemTmp = {logDevCounter, pLogDev};

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
      for( uint8_t itr = 0; itr < logDevCounter; itr++ )
      {
         IxLogDevice *pDevice = LOGDEV_LIST[itr].pLogDevice;

         if( NULL != pDevice )
         {
            pDevice->Process();
         }
      }
   }

   sleep_mcs(50);
}
