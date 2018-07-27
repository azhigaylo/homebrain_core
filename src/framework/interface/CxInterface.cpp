//------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "interface/CxInterface.h"
#include "interface/CxInterfaceManager.h"
//------------------------------------------------------------------------------

CxInterface::CxInterface( const char *name ):
   IxInterface()
{
   strncpy( pcInterfaceName, const_cast<char*>(name), sizeof(pcInterfaceName) );
   registration();
}

CxInterface::~CxInterface()
{
   printDebug("CxInterface/%s: Interface = %s removed ", __FUNCTION__, pcInterfaceName);
}

void CxInterface::registration()
{
  pCxInterfaceManager pInterfaceManager = CxInterfaceManager::getInstance();

  pInterfaceManager->set_interface( this );
}

const char *CxInterface::getInterfaceName()
{
   return pcInterfaceName;
}
