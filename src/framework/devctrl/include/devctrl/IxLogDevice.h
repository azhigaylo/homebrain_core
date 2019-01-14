/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _IX_LOGDEVICE
#define _IX_LOGDEVICE

//------------------------------------------------------------------------------

class IxLogDevice
{
 public:

   IxLogDevice(){}
   virtual ~IxLogDevice(){}

   virtual void open()  = 0;
   virtual void close() = 0;
   virtual const char *getDeviceName() = 0;
   virtual const char *getInterfaceName() = 0;
   virtual void start() = 0;

   virtual bool Process() = 0;
   virtual uint16_t getDevStatus( ) = 0;
   virtual void setDevStatus( uint16_t status ) = 0;

}; typedef IxLogDevice *pIxLogDevice;

#endif /*_IX_LOGDEVICE*/
