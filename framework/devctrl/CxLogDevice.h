#ifndef _CX_LOGDEVICE
#define _CX_LOGDEVICE

#include "IxLogDevice.h"

//------------------------------------------------------------------------------
   
class CxLogDevice : public IxLogDevice
{
 public: 
   
   // 
   virtual void  SwitchOn();
   virtual void  SwitchOff();      
   virtual char* GetDeviceName();
   virtual void  SetCommunicationPort( char* pPortName, unsigned long PortRate, char PortParity );
   virtual void  Start() = 0;   
   
 protected:

   CxLogDevice( char *deviceName );
   ~CxLogDevice(){}
   
   // com port settings
   char PORT_NAME[20];
   unsigned long BaudRate;
   char Parity;  
   int PORT;
   
   bool enable_operation;                                                       // true - enable / false - disable    
   
 private:    
   
   void DeviceRegistration();
   
   char DEVICE_NAME[20]; 
   
}; typedef CxLogDevice *pCxLogDevice;

#endif /*_CX_LOGDEVICE*/   
