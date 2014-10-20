#include "utils.h"
#include "PSysI.h"
#include "CxLogDevice.h"
#include "CxLogDeviceManager.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxLogDevice::CxLogDevice( char *deviceName ):
  IxLogDevice( ),
  BaudRate( 9600 ),
  Parity( 0 ),
  PORT( -1 ),  
  enable_operation( false )
{  
   mod_strncpy( DEVICE_NAME, deviceName, sizeof(DEVICE_NAME) );   
   mod_memset( PORT_NAME, 0, sizeof(PORT_NAME), sizeof(PORT_NAME) );
   DeviceRegistration();
}

void CxLogDevice::DeviceRegistration()
{
  CxLogDeviceManager &DeviceManager = CxLogDeviceManager::getInstance();
  DeviceManager.logDevRegistration( this );
}

char* CxLogDevice::GetDeviceName()
{
  return DEVICE_NAME;
}

void CxLogDevice::SetCommunicationPort( char* pPortName, unsigned long PortRate, char PortParity )
{
   mod_strncpy( PORT_NAME, pPortName, sizeof(PORT_NAME) );
   BaudRate = PortRate;
   Parity   = PortParity;      
}

void CxLogDevice::SwitchOn()
{
   DBG_SCOPE( CxLogDevice, CxLogDevice )
   
   DBG_MSG( ("[M] LogDevice - %s <ON> request\n\r", DEVICE_NAME) );    
     
   // create file "COMx"
   PORT = FileCreate( PORT_NAME, FA_WRITE );
   
   if( PORT != -1 )
   {  
     // set properties
     DCB portProperties;
     GetCommState( PORT, &portProperties );
     
     portProperties.BaudRate = BaudRate;   
     portProperties.Parity   = Parity; 
         
     SetCommState( PORT, &portProperties );       
   }     

   enable_operation = true;
}

void CxLogDevice::SwitchOff()
{  
  DBG_SCOPE( CxLogDevice, CxLogDevice )
  
  DBG_MSG( ("[M] LogDevice - %s <OFF> request\n\r", DEVICE_NAME) );   
    
  if( PORT != -1 ) 
  {      
    FileClose( PORT );
    PORT = -1;
    
    enable_operation = false;
  }  
}

