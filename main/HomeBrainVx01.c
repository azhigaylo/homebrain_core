//------------------------------------------------------------------------------
// Standard includes. 

//------------------------------------------------------------------------------
// driver includes
#include "CxLauncher.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

int main( void )
{
   // When using the JTAG debugger the hardware is not always initialised to
   // the correct default state.  This line just ensures that this does not
   // cause all interrupts to be masked at the start. 
   AT91C_BASE_AIC->AIC_EOICR = 0;          
   
   CxLauncher &Launcher =  CxLauncher::getInstance();

   Launcher.Start();  

   return 0;
} 


