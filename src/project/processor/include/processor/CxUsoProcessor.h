#ifndef _CX_USO_PROCESSOR
#define _CX_USO_PROCESSOR

//------------------------------------------------------------------------------
#include <unistd.h>
#include <vector>

#include "common/ptypes.h"
#include "os_wrapper/IxRunnable.h"
#include "devctrl/IxLogDevice.h"
#include "devctrl/CxLogDeviceManager.h"
//------------------------------------------------------------------------------
#define logDevItemsSize 5
//------------------------------------------------------------------------------

class CxUsoProcessor : public IxRunnable
{
   public:

      CxUsoProcessor( const char * sPrcName, const char *sInterfaceNmae );
      virtual ~CxUsoProcessor( );

      bool set_logdev( IxLogDevice * pLogDev );
      char *get_interfacename( )const;

   protected:

      virtual void TaskProcessor();

   private:

      char *sUsedInterface;
      uint16_t logDevCounter;
      std::vector<TLogDevListItem> LOGDEV_LIST;
};

//------------------------------------------------------------------------------

#endif // _CX_USO_PROCESSOR
