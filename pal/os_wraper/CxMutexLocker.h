#ifndef _CX_MUTEX_LOCKER
#define _CX_MUTEX_LOCKER

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "utils.h"
#include "CxMutex.h"
//------------------------------------------------------------------------------

class QMutexLocker
{
public:
  
   QMutexLocker( CxMutex *mutex );
   ~QMutexLocker( );

   CxMutex * mutex( );
   void unlock( );

private:

   QMutexLocker( const QMutexLocker& rhs );
   QMutexLocker& operator=( const QMutexLocker& rhs );

};

#endif

