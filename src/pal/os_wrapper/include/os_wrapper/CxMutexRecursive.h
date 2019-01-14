/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_MUTEX
#define _CX_MUTEX
//------------------------------------------------------------------------------
#include <pthread.h>
//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "common/utils.h"
//------------------------------------------------------------------------------
#define configMAX_MUTEX_NAME_LEN 50
//------------------------------------------------------------------------------

class CxMutexRecursive
{
public:

   CxMutexRecursive( const char *name );
   ~CxMutexRecursive( );

   void lock( );
   void unlock( );

private:

   CxMutexRecursive( const CxMutexRecursive& rhs );
   CxMutexRecursive& operator=( const CxMutexRecursive& rhs );

protected:

   pthread_mutex_t mutex;
   char mutexName[configMAX_MUTEX_NAME_LEN];
};

#endif

