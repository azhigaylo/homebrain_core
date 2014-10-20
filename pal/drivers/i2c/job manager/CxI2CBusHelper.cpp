//------------------------------------------------------------------------------

#include "CxI2CBusHelper.h"
#include "i2c_master_drv.h"

//------------------------------------------------------------------------------

CxI2CBusHelper::CxI2CBusHelper( void (*execution_func)(pTI2CRecBlock) ):
   call_back  ( execution_func )
  ,helperBusy ( false          ) 
{    
  vI2C_SerialPortInit( 1000, 10 );
}

//------------------------------------------------------------------------------
bool CxI2CBusHelper::IsHelperBusy()
{  
  return helperBusy;
}

bool CxI2CBusHelper::WriteMultiple( int SlaveAddr, unsigned int NumOfBytes, char *data )
{
  unsigned char errorCounter = 0;
  
  helperBusy = true;  
  
  // job execution
  errorCounter = TWI_WriteMultiple( AT91C_BASE_TWI, (unsigned char)SlaveAddr, 0, 0, data, NumOfBytes );  

  // call back call 
  (*call_back)(&i2cRecBlock);
  
  helperBusy = false;    
  
  return (errorCounter == 0);
}
  
bool CxI2CBusHelper::ReadMultiple( int SlaveAddr, unsigned int NumOfBytes )
{
  int errorCounter = 0;
  
  helperBusy = true;  
   
  // job execution  
  //errorCounter = AT91F_TWI_ReadMultiple( AT91C_BASE_TWI, SlaveAddr, NumOfBytes, i2cRecBlock.rdData );
  
  if( errorCounter == 0 )
  {
     i2cRecBlock.rdLength = NumOfBytes;  
  }
  else 
  {
     i2cRecBlock.rdLength = 0;
  }  
  
  // call back call 
  (*call_back)(&i2cRecBlock);
  
  helperBusy = false;     

  return (errorCounter == 0);
}
//------------------------------------------------------------------------------

