#ifndef _IX_INTERFACE
#define _IX_INTERFACE

//------------------------------------------------------------------------------
#include "ptypes.h"
//------------------------------------------------------------------------------

class IxInterface
{
   public:

      IxInterface(){}
      virtual ~IxInterface(){}

      virtual int32_t open  ( ) = 0;        // open or create a interface
      virtual int32_t close ( ) = 0;        // close interface
      virtual const char *getInterfaceName() = 0;

}; typedef IxInterface *pIxInterface;

#endif /*_IX_INTERFACE*/
