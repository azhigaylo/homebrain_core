#ifndef _CX_INTERFACE
#define _CX_INTERFACE

#include "IxInterface.h"
//------------------------------------------------------------------------------
#define configMAX_INTERFACE_NAME_LEN 50
//------------------------------------------------------------------------------

class CxInterface : public IxInterface
{
   public:

      const char *getInterfaceName();

      virtual int32_t open  ( ){}   // !!! should be fixed 
      virtual int32_t close ( ){}   // !!! should be fixed 

   //protected:   // !!! should be fixed 

      CxInterface( const char *name );
      virtual ~CxInterface(){}

      void registration();

   private:

      char pcInterfaceName[configMAX_INTERFACE_NAME_LEN];

}; typedef CxInterface *pCxInterface;

#endif /*_CX_INTERFACE*/   