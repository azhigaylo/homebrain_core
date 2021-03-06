/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

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
   ,LOGDEV_LIST    ( )
{
   sUsedInterface = strdup(sInterfaceNmae);

   printDebug("CxUsoProcessor/%s: Processor for %s created", __FUNCTION__, sUsedInterface );
}

CxUsoProcessor::~CxUsoProcessor( )
{
   // delete my own task
   task_stop( );

   printDebug("CxUsoProcessor/%s: Processor for %s removed", __FUNCTION__, sUsedInterface );

   if (0 != sUsedInterface)
   {
      free( sUsedInterface );
   }

}

//------------------------------------------------------------------------------

bool CxUsoProcessor::set_logdev( IxLogDevice *pLogDev )
{
   // add pointer on logdev in vector
   TLogDevListItem logDevListItemTmp = {logDevCounter, pLogDev};

   LOGDEV_LIST.push_back( logDevListItemTmp );

   printDebug("CxUsoProcessor/%s: LogDev = %s add OK ", __FUNCTION__, pLogDev->getDeviceName());

   return true;
}

char *CxUsoProcessor::get_interfacename( )const
{
   return sUsedInterface;
}

//------------------------------------------------------------------------------

void CxUsoProcessor::TaskProcessor()
{
   uint16_t errCounter = 0;

   for( uint8_t itr = 0; itr < LOGDEV_LIST.size(); itr++ )
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
   if (errCounter == LOGDEV_LIST.size())
   {
       sleep_mcs(200000);
   }

   sleep_mcs(50);
}
