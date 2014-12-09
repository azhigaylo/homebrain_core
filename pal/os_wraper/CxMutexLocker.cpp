//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "QMutexLocker.h"
//------------------------------------------------------------------------------

QMutexLocker::QMutexLocker( CxMutex *mutex )
{
   if (0 != mutex)
   {
      mutex.lock();
   }
   else
   {
     printError("QMutexLocker/%s: mutex=0", __FUNCTION__);
   }
}

QMutexLocker::~QMutexLocker( )
{
   if (0 != mutex)
   {
      mutex.unlock();
   }
   else
   {
     printError("QMutexLocker/%s: mutex=0", __FUNCTION__);
   }
}
 
void QMutexLocker::unlock( )
{
   if (0 != mutex)
   {
      mutex.unlock();
   }
   else
   {
     printError("QMutexLocker/%s: mutex=0", __FUNCTION__);
   } 
}