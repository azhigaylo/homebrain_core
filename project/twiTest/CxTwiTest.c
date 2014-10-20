#include "CxTwiTest.h"


//------------------------------------------------------------------------------
/*
/// Describes the type and attribute of one PIO pin or a group of similar pins.
typedef struct {
    /// Bitmask indicating which pin(s) to configure.
    unsigned int mask; 
    /// Pointer to the PIO controller which has the pin(s).
    AT91S_PIO    *pio;
    /// Peripheral ID of the PIO controller which has the pin(s).
    unsigned char id;
    /// Pin type (see "Pin types").
    unsigned char type;
    /// Pin attribute (see "Pin attributes").
    unsigned char attribute;
} Pin;
*/
//------------------------------------------------------------------------------

CxTwiTest &tTwiTest= CxTwiTest::getInstance();


CxTwiTest::CxTwiTest():
   IxRunnable ( "TWI_TASK" ) 
{
  pinDataA2.mask = 0x00000020;
  pinDataA2.pio  = AT91C_BASE_PIOA;
  pinDataA2.id   = AT91C_ID_PIOA;
  pinDataA2.type = PIO_INPUT;
  pinDataA2.attribute = PIO_PULLUP;
  
  PIO_Configure( &pinDataA2, 1 );    
  PIO_InitializeInterrupts( 2 );  
  PIO_EnableIt( &pinDataA2 );
  
/*  
  pinDataA2.mask = 0x00000020;
  pinDataA2.pio  = AT91C_BASE_PIOA;
  pinDataA2.id   = AT91C_ID_PIOA;
  pinDataA2.type = PIO_INPUT;
  pinDataA2.attribute = PIO_PULLUP;
*/  
  
  task_run();
}

//------------------------------------------------------------------------------

CxTwiTest &CxTwiTest::getInstance()
{
  static CxTwiTest theInstance;
  return theInstance;
}


void CxTwiTest::TaskProcessor()
{

}
