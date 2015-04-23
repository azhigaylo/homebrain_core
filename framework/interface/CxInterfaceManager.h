#ifndef _CX_INTERFACE_MANAGER
#define _CX_INTERFACE_MANAGER

//------------------------------------------------------------------------------
#include "CxList.h"
#include "CxMutex.h"
#include "IxInterface.h"
//------------------------------------------------------------------------------
#define interfaceListSize 5
//------------------------------------------------------------------------------

struct TInterfaceItem
{
  uint16_t ID;
  pIxInterface pInterface;
};

typedef TInterfaceItem *pInterfaceItem;

//------------------------------------------------------------------------------

class CxInterfaceManager
{
   public:

      static CxInterfaceManager * getInstance();
      void delInstance();

      bool set_interface( IxInterface * pNewInterface );
      IxInterface *get_interface( const char *name );

   private:

      CxInterfaceManager( );
      ~CxInterfaceManager( ){}

      uint16_t interfaceCounter;
      CxList<TInterfaceItem> CONNECTION_LIST;

      static CxMutex singlInterfaceLock;
      static CxInterfaceManager* theInstance;

      CxInterfaceManager( const CxInterfaceManager& );                 // Prevent copy-construction
      CxInterfaceManager& operator=( const CxInterfaceManager& );      // Prevent assignment
  
}; typedef CxInterfaceManager * pCxInterfaceManager;

#endif /*_CX_INTERFACE_MANAGER*/
