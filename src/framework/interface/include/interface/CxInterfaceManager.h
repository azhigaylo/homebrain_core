#ifndef _CX_INTERFACE_MANAGER
#define _CX_INTERFACE_MANAGER

//------------------------------------------------------------------------------
#include <vector>

#include "os_wrapper/CxMutex.h"
#include "IxInterface.h"
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
      static void delInstance();

      bool set_interface( IxInterface * pNewInterface );
      IxInterface *get_interface( const char *name );

   private:

      CxInterfaceManager( );
      ~CxInterfaceManager( ){}

      void clr_interface_list();

      uint16_t interfaceCounter;
      std::vector<TInterfaceItem> CONNECTION_LIST;

      static CxMutex singlInterfaceLock;
      static CxInterfaceManager* theInstance;

      CxInterfaceManager( const CxInterfaceManager& );                 // Prevent copy-construction
      CxInterfaceManager& operator=( const CxInterfaceManager& );      // Prevent assignment

}; typedef CxInterfaceManager * pCxInterfaceManager;

#endif /*_CX_INTERFACE_MANAGER*/
