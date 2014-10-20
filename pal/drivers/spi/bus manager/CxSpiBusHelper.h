#ifndef _CX_SPI_BUS_HELPER
#define _CX_SPI_BUS_HELPER

//------------------------------------------------------------------------------

#include "FreeRTOS.h"

//------------------------------------------------------------------------------
// at here we have to wrote all common SPI function
//------------------------------------------------------------------------------


class CxSpiBusHelper 
{ 
 public: 
   
   CxSpiBusHelper( );
   ~CxSpiBusHelper( ){}   

   void init_spi();
     
   unsigned char xmit_spi(unsigned char dat);
   
   unsigned char rcvr_spi();
   
   void rcvr_spi_m(unsigned char *dest);
   
   void setSpeed( char chipNumber, unsigned char speed);
   
   void setSpiPropertis( char chipNumber, unsigned int csRegister );
   
   void chipSelect( char chipNumber );
   
   // for SD use
   bool rcvr_datablock( unsigned char *buff, unsigned char wc );
   bool xmit_datablock( const unsigned char *buff, unsigned char token );
          
 private:   

   bool helperBusy;
}; 

typedef CxSpiBusHelper *pCxSpiBusHelper;

#endif /*_CX_SPI_BUS_HELPER*/   
