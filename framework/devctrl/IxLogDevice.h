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
   virtual void start() = 0;
   
}; typedef IxLogDevice *pIxLogDevice;

#endif /*_IX_LOGDEVICE*/   
