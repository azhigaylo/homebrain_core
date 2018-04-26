#ifndef _CX_MUTEX
#define _CX_MUTEX
//------------------------------------------------------------------------------
#include <pthread.h>
//------------------------------------------------------------------------------
#include "ptypes.h"
#include "utils.h"
//------------------------------------------------------------------------------
#define configMAX_MUTEX_NAME_LEN 50
//------------------------------------------------------------------------------

class CxMutex
{
public:
  
   CxMutex( const char *name );
   ~CxMutex( );

   void lock( );
   bool tryLock( );
   void unlock( );

private:

   CxMutex( const CxMutex& rhs );
   CxMutex& operator=( const CxMutex& rhs );

protected:
     
   pthread_mutex_t mutex; 
   char mutexName[configMAX_MUTEX_NAME_LEN];
};

#endif

