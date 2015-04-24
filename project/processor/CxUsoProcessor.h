#ifndef _CX_USO_PROCESSOR
#define _CX_USO_PROCESSOR

//------------------------------------------------------------------------------
#include <unistd.h>

#include "ptypes.h"
#include "CxVector.h"
#include "IxRunnable.h"
#include "IxLogDevice.h"
#include "CxLogDeviceManager.h"
//------------------------------------------------------------------------------
#define logDevItemsSize 5
//------------------------------------------------------------------------------

class CxUsoProcessor : public IxRunnable
{
   public:

      CxUsoProcessor( const char *sInterfaceNmae );
      virtual ~CxUsoProcessor( );

      bool set_logdev( IxLogDevice * pLogDev );
      char *get_interfacename( )const;
         
   protected:
   
      virtual void TaskProcessor();


   private:
      
      char *sUsedInterface;
      uint16_t logDevCounter;
      CxVector<TLogDevListItem> LOGDEV_LIST;   
};

//------------------------------------------------------------------------------

#endif // _CX_USO_PROCESSOR