//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxInterface.h"
#include "CxLogDevice.h"
#include "CxLogDeviceManager.h"
//------------------------------------------------------------------------------

CxLogDevice::CxLogDevice( const char *deviceName, const char *interfaceName ):
  IxLogDevice( )
{
   strncpy_m( pcDeviceName, const_cast<char*>(deviceName), sizeof(pcDeviceName) );
   strncpy_m( pcInterfaceName, const_cast<char*>(interfaceName), sizeof(pcInterfaceName) );

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

const char *CxLogDevice::getDeviceName()
{
   return pcDeviceName;
}

const char *CxLogDevice::getInterfaceName()
{
   return pcInterfaceName;
}
   