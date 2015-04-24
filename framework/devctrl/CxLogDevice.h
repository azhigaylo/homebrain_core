#ifndef _CX_LOGDEVICE
#define _CX_LOGDEVICE

//------------------------------------------------------------------------------
#include "ptypes.h"
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

   virtual void Process(){} // logical device processor
   
 //protected:   // !!! should be fixed 

   virtual ~CxLogDevice();
   CxLogDevice( const char *deviceName, const char *interfaceName );
   
 private:    
   
   void registration();
   
   char pcDeviceName[configMAX_DEVICE_NAME_LEN]; 
   char pcInterfaceName[configMAX_DEVICE_NAME_LEN]; 
   
}; typedef CxLogDevice *pCxLogDevice;

#endif /*_CX_LOGDEVICE*/   
