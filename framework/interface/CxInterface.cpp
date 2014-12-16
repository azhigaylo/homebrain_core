//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxInterface.h"
#include "CxInterfaceManager.h"
//------------------------------------------------------------------------------

CxInterface::CxInterface( const char *name ):
   IxInterface()
{
   strncpy_m( pcInterfaceName, const_cast<char*>(name), sizeof(pcInterfaceName) );
   registration();     
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