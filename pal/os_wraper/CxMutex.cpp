//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxMutex.h"
//------------------------------------------------------------------------------

CxMutex::CxMutex( const char *name )
{
   strncpy_m( mutexName, const_cast<char*>(name), configMAX_MUTEX_NAME_LEN );

   pthread_mutexattr_t mutexattr;
   
   pthread_mutexattr_init(&mutexattr);
   pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);

   if (0 != pthread_mutex_init(&mutex,&mutexattr))
   {
      printError("CxMutex/%s: mutex=%s init error !!!", __FUNCTION__, mutexName);
   }
   else
   {
     printDebug("CxMutex/%s: mutex=%s created", __FUNCTION__, mutexName);
   }
}

CxMutex::~CxMutex( )
{
   if (0 != pthread_mutex_destroy(&mutex))
   {
      printError("CxMutex/%s: mutex=%s destroy error !!!", __FUNCTION__, mutexName);
   }  
   {
     printDebug("CxMutex/%s: mutex=%s destroyed", __FUNCTION__, mutexName);
   }
}

void CxMutex::lock( )
{
   if (0 != pthread_mutex_lock(&mutex))
   {
      printError("CxMutex/%s: mutex=%s lock error !!!", __FUNCTION__, mutexName);
   }  
}

bool CxMutex::tryLock( )
{
   bool result = true;

   if (0 != pthread_mutex_trylock(&mutex))
   {
      printError("CxMutex/%s: trylock=%s lock error !!!", __FUNCTION__, mutexName);
      result = false;
   }

   return result;
}
   
void CxMutex::unlock( )
{
   if (0 != pthread_mutex_unlock(&mutex))
   {
      printError("CxMutex/%s: mutex=%s unlock error !!!", __FUNCTION__, mutexName);
   }  
}