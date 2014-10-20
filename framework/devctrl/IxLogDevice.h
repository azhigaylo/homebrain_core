#ifndef _IX_LOGDEVICE
#define _IX_LOGDEVICE

//------------------------------------------------------------------------------

class IxLogDevice
{
 public: 

   IxLogDevice();
   ~IxLogDevice(){}
   
   // 
   virtual void SwitchOn()  = 0;
   virtual void SwitchOff() = 0;      
   virtual char *GetDeviceName() = 0; 
   virtual void SetCommunicationPort( char* pPortName, unsigned long PortRate, char PortParity ) = 0;
   virtual void Start() = 0;
   
}; typedef IxLogDevice *pIxLogDevice;

#endif /*_IX_LOGDEVICE*/   
