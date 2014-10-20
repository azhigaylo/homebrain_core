//------------------------------------------------------------------------------
#include "spi_master_drv.h"
#include "CxSpiBusHelper.h"

//------------------------------------------------------------------------------

CxSpiBusHelper::CxSpiBusHelper( void (*execution_func)(pTI2CRecBlock) ):
   call_back  ( execution_func )
  ,helperBusy ( false          ) 
{    
  disk_initialize();
}

//------------------------------------------------------------------------------
bool CxSpiBusHelper::IsHelperBusy()
{  
  return helperBusy;
}

bool CxSpiBusHelper::WriteMultiple( int SlaveAddr, unsigned int NumOfBytes, char *data )
{
  unsigned char errorCounter = 0;
  
  helperBusy = true;  
  
  // job execution
  //errorCounter = TWI_WriteMultiple( AT91C_BASE_TWI, (unsigned char)SlaveAddr, 0, 0, data, NumOfBytes );  

  // call back call 
  //(*call_back)(&i2cRecBlock);
  
  helperBusy = false;    
  
  return (errorCounter == 0);
}
  
bool CxSpiBusHelper::ReadMultiple( int SlaveAddr, unsigned int NumOfBytes )
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
  //(*call_back)(&i2cRecBlock);
  
  helperBusy = false;     

  return (errorCounter == 0);
}
//------------------------------------------------------------------------------

