#ifndef _CX_MUTEX_LOCKER
#define _CX_MUTEX_LOCKER

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "utils.h"
#include "CxMutex.h"
//------------------------------------------------------------------------------

class CxMutexLocker
{
public:

   CxMutexLocker( CxMutex *mtx );
   ~CxMutexLocker( );

   CxMutex * getmutex( );
   void unlock( );

private:

   CxMutexLocker( const CxMutexLocker& rhs );
   CxMutexLocker& operator=( const CxMutexLocker& rhs );
   
   CxMutex *mutex;

};

#endif

