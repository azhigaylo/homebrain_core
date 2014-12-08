//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutexRecursive.h"
//------------------------------------------------------------------------------

CxMutexRecursive::CxMutexRecursive( const char *name )
{
   strncpy_m( &mutexName[1], const_cast<char*>(name), configMAX_MUTEX_NAME_LEN );

   pthread_mutexattr_t mutexattr;
   
   pthread_mutexattr_init(&mutexattr);
   pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

   if (0 != pthread_mutex_init(&mutex,&mutexattr))
   {
      printError("CxMutexRecursive/%s: mutex=%s init error !!!", __FUNCTION__, mutexName);
   }
   
}

CxMutexRecursive::~CxMutexRecursive( )
{
   if (0 != pthread_mutex_destroy(&mutex))
   {
      printError("CxMutexRecursive/%s: mutex=%s destroy error !!!", __FUNCTION__, mutexName);
   }  
}

void CxMutexRecursive::lock( )
{
   if (0 != pthread_mutex_lock(&mutex))
   {
      printError("CxMutexRecursive/%s: mutex=%s lock error !!!", __FUNCTION__, mutexName);
   }  
}

void CxMutexRecursive::unlock( )
{
   if (0 != pthread_mutex_unlock(&mutex))
   {
      printError("CxMutexRecursive/%s: mutex=%s unlock error !!!", __FUNCTION__, mutexName);
   }  
}

