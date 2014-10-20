#include "utils.h"
#include "AT91SAM7S128.h"
#include "at91sam7s128\lib_AT91SAM7S.h"
#include "CxAssertion.h"

//------------------------------------------------------------------------------
// Hook functions
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
   CxAssertion &Caller = CxAssertion::getInstance();
   Caller.CallAssertion( AssertionStack, reinterpret_cast<const char*>(pcTaskName) );  
}

void vApplicationTaskCreateFailedHook(void)
{
   CxAssertion &Caller = CxAssertion::getInstance();
   Caller.CallAssertion( AssertionTask );  
}

void vApplicationMutexCreateFailedHook(void)
{
   CxAssertion &Caller = CxAssertion::getInstance();
   Caller.CallAssertion( AssertionMutex );  
}

void vApplicationQueueCreateFailedHook(void)
{
   CxAssertion &Caller = CxAssertion::getInstance();
   Caller.CallAssertion( AssertionQueue );  
}

void vApplicationStaticPoolFailedHook(void)
{
   CxAssertion &Caller = CxAssertion::getInstance();
   Caller.CallAssertion( AssertionPool );  
}
//------------------------------------------------------------------------------

CxAssertion::CxAssertion( )
{  
  mod_memset( dataAssertion, 0, AssertionLength, AssertionLength );
} 

CxAssertion::~CxAssertion( )
{

}

CxAssertion &CxAssertion::getInstance( )
{
  static CxAssertion theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------
// Assertion
void CxAssertion::CallAssertion( const char* textOfAssertion )
{
  mod_strncpy( dataAssertion, textOfAssertion, AssertionLength );
  
  ApplicationHalt();
}

void CxAssertion::CallAssertion( const char* textOfAssertion, const char* textAdditional )
{
  mod_strncpy( dataAssertion, textOfAssertion, AssertionLength );
  mod_sprintf( dataAssertion, AssertionLength, "%s - %s", textOfAssertion, textAdditional );
  
  ApplicationHalt();
}

void CxAssertion::ApplicationHalt()
{
  portENTER_CRITICAL();
  {	
    LowUSARTinit();
    PrintAssertionInUART();
    while( true );    
  }
}

//------------------------------------------------------------------------------
// USART initialization
void CxAssertion::LowUSARTinit()
{
  /* Enable the USART clock. */
  AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_US0 );

  AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, ( ( unsigned portLONG ) AT91C_PA6_TXD0 ), 0 );

  /* Set the required protocol. */
  AT91F_US_Configure( ( ( AT91PS_USART ) AT91C_BASE_US0 ), configCPU_CLOCK_HZ, AT91C_US_ASYNC_MODE, 115200, 0 );

  /* Enable Tx. */
  ( ( AT91PS_USART ) AT91C_BASE_US0 )->US_CR = AT91C_US_TXEN;
}

void CxAssertion::PrintAssertionInUART()
{
  write_str_USART0( "\n\r" );
  write_str_USART0( dataAssertion );
  write_str_USART0( "\n\r" );  
}

void CxAssertion::write_str_USART0(unsigned char* buff) 
{
  unsigned short i = 0x0;
  unsigned short str_len = mod_strlen( buff, AssertionLength );
    
  while( i < str_len ) 
  {
    write_char_USART0( buff[i] );
    i++;
  }
}

void CxAssertion::write_char_USART0(unsigned char ch)
{
  while( !(AT91C_BASE_US0->US_CSR&AT91C_US_TXRDY)==1 );
  AT91C_BASE_US0->US_THR = (ch & 0x1FF);
}
