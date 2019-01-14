/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_MUTEX_LOCKER
#define _CX_MUTEX_LOCKER

//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "common/utils.h"
#include "os_wrapper/CxMutex.h"
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

