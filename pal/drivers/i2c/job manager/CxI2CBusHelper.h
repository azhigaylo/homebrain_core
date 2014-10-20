#ifndef _CX_I2C_BUS_HELPER
#define _CX_I2C_BUS_HELPER


//------------------------------------------------------------------------------
#pragma pack ( 1 )
/*
struct TI2IOBUFFER
{
  unsigned char  status;
           char  wrtData[50];  
};  
typedef TI2IOBUFFER *pTI2IOBUFFER;
*/
struct TI2CRecBlock
{
  unsigned char  rdLength;
           char  rdData[50];  
};  
typedef TI2CRecBlock *pTI2CRecBlock;

#pragma pack ( )


//------------------------------------------------------------------------------

class CxI2CBusHelper 
{ 
 public: 
   
   CxI2CBusHelper( void (*execution_func)(pTI2CRecBlock) );
   ~CxI2CBusHelper( ){}   

   bool IsHelperBusy( );
   
   bool WriteMultiple( int SlaveAddr, unsigned int NumOfBytes, char *data );
  
   bool ReadMultiple( int SlaveAddr, unsigned int NumOfBytes );

 private:   
   
   void (*call_back)(pTI2CRecBlock);                                            // execution function 
   
   TI2CRecBlock i2cRecBlock;
   
   bool helperBusy;
}; 

typedef CxI2CBusHelper *pCxI2CBusHelper;

#endif /*_CX_I2C_BUS_HELPER*/   
