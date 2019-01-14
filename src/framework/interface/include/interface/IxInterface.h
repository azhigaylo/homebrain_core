/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _IX_INTERFACE
#define _IX_INTERFACE

//------------------------------------------------------------------------------
#include "common/ptypes.h"
//------------------------------------------------------------------------------

class IxInterface
{
   public:

      IxInterface(){}
      virtual ~IxInterface(){}

      virtual int32_t open  ( ) = 0;        // open or create a interface
      virtual int32_t close ( ) = 0;        // close interface
      virtual const char *getInterfaceName() = 0;

}; typedef IxInterface *pIxInterface;

#endif /*_IX_INTERFACE*/
