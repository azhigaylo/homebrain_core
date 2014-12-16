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

CxLogDevice::CxLogDevice( const char *deviceName ):
  IxLogDevice( )
{
   strncpy_m( pcDeviceName, const_cast<char*>(deviceName), sizeof(pcDeviceName) );
   registration();
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