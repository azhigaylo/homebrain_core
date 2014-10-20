#include "utils.h"
#include "CxBlinking_drv.h"
#include "..\usart\CxUsart_1_drv.h"
#include "at91sam7s128\lib_AT91SAM7S.h"

//------------------------------------------------------------------------------
//bits
#define   BIT17       0x00020000
#define   BIT18       0x00040000

// commands
#define   Y_ON          1
#define   Y_OFF         2
#define   G_ON          3
#define   G_OFF         4

//------------------------------------------------------------------------------
CxBlinking_drv &b_drv = CxBlinking_drv::getInstance();
//------------------------------------------------------------------------------

CxBlinking_drv::CxBlinking_drv( ):
  IxDriver( "BLINK_TASK" )
{
   // enable the clock of the PIO
   AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
   // configure the PIO Lines corresponding to LED1 & LED1
   AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, BIT17 | BIT18 );
   AT91F_PIO_SetOutput( AT91C_BASE_PIOA, BIT17 | BIT18 );
} 

//------------------------------------------------------------------------------

CxBlinking_drv::~CxBlinking_drv( )
{

}

//------------------------------------------------------------------------------

CxBlinking_drv &CxBlinking_drv::getInstance( )
{
  static CxBlinking_drv theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxBlinking_drv::CommandProcessor( TCommand &Command )
{
    switch ( Command.ComID )
    {
      case Y_ON  : 
        {
          AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, BIT17 );
          break;
        }        
      case Y_OFF : 
        {
          AT91F_PIO_SetOutput( AT91C_BASE_PIOA, BIT17 );
          break;
        }   
      case G_ON  : 
        {
          AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, BIT18 );
          break;
        }   
      case G_OFF : 
        {
          AT91F_PIO_SetOutput( AT91C_BASE_PIOA, BIT18 );
          break;
        }               
      default :  { /* unknown command */ }
    }
}
