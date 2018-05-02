//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "interface/CxInterface.h"
#include "devctrl/CxLogDevice.h"
#include "devctrl/CxLogDeviceManager.h"
//------------------------------------------------------------------------------

CxLogDevice::CxLogDevice( const char *deviceName, const char *interfaceName ):
   IxLogDevice( )
  ,devStatus  ( 1 )
{
   strncpy( pcDeviceName, const_cast<char*>(deviceName), sizeof(pcDeviceName) );
   strncpy( pcInterfaceName, const_cast<char*>(interfaceName), sizeof(pcInterfaceName) );

   registration();
}

CxLogDevice::~CxLogDevice()
{
   printDebug("CxLogDevice/%s: LogDev = %s removed ", __FUNCTION__, pcDeviceName);
}

void CxLogDevice::registration()
{
  pCxLogDeviceManager pLogDeviceManager = CxLogDeviceManager::getInstance();

  pLogDeviceManager->set_logdev( this );
}

uint16_t CxLogDevice::getDevStatus( )
{
   return devStatus;
}

void CxLogDevice::setDevStatus( uint16_t status )
{
   devStatus = status;
}

const char *CxLogDevice::getDeviceName()
{
   return pcDeviceName;
}

const char *CxLogDevice::getInterfaceName()
{
   return pcInterfaceName;
}
