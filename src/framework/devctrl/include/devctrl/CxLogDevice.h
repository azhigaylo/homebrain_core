/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_LOGDEVICE
#define _CX_LOGDEVICE

//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "IxLogDevice.h"
//------------------------------------------------------------------------------
#define configMAX_DEVICE_NAME_LEN 50
//------------------------------------------------------------------------------


class CxLogDevice : public IxLogDevice
{
 public:

   virtual const char *getDeviceName();
   virtual const char *getInterfaceName();

   virtual void open(){}    // !!! should be fixed
   virtual void close(){}   // !!! should be fixed
   virtual void start(){}   // !!! should be fixed

   virtual bool Process(){return false;} // logical device processor

   virtual uint16_t getDevStatus( );
   virtual void setDevStatus( uint16_t status );

 protected:

   virtual ~CxLogDevice();
   CxLogDevice( const char *deviceName, const char *interfaceName );

 private :

   void registration();

   char pcDeviceName[configMAX_DEVICE_NAME_LEN];
   char pcInterfaceName[configMAX_DEVICE_NAME_LEN];

   uint16_t devStatus;          // USO_Status_OK(0) or USO_Status_NoReply(1)

}; typedef CxLogDevice *pCxLogDevice;

#endif /*_CX_LOGDEVICE*/
