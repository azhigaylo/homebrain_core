/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutexLocker.h"
//------------------------------------------------------------------------------

CxMutexLocker::CxMutexLocker( CxMutex *mtx ):
   mutex( mtx )
{
   if (0 != mutex)
   {
      mutex->lock();
   }
   else
   {
     printError("CxMutexLocker/%s: mutex=0", __FUNCTION__);
   }
}

CxMutexLocker::~CxMutexLocker( )
{
   if (0 != mutex)
   {
      mutex->unlock();
   }
   else
   {
     printError("CxMutexLocker/%s: mutex=0", __FUNCTION__);
   }
}

void CxMutexLocker::unlock( )
{
   if (0 != mutex)
   {
      mutex->unlock();
   }
   else
   {
     printError("CxMutexLocker/%s: mutex=0", __FUNCTION__);
   }
}

CxMutex * CxMutexLocker::getmutex( )
{
   return mutex;
}
