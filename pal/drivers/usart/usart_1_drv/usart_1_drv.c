/* Standard includes. */ 
#include <stdlib.h>

/* Demo application includes. */
#include "..\usart_1_drv.h"
#include "at91sam7s128\lib_AT91SAM7S.h"

/*-----------------------------------------------------------*/

/* Location of the COM0 registers. */
#define serCOM0						( ( AT91PS_USART ) AT91C_BASE_US0 )

/* Interrupt control macros. */
#define serINTERRUPT_LEVEL				( 5 )
#define vU1_InterruptOn()				AT91F_US_EnableIt( serCOM0, AT91C_US_TXRDY | AT91C_US_RXRDY )
#define vU1_InterruptOff()				AT91F_US_DisableIt( serCOM0, AT91C_US_TXRDY )

// Queues used to hold received characters, and characters waiting to be transmitted. 

static xQueueHandle xRxedChars  = NULL; 
static xQueueHandle xCharsForTx = NULL; 

static bool bTransmitterState   = false;   // true - transmission finished / false - transmission in process
/*-----------------------------------------------------------*/

bool vU1_isTransmissionFinished( xComPortHandle xPort )
{
  bool result = false;
  
  if( xPort != NULL )
  {
    result = bTransmitterState;
    
    if( true == result )
    {
      bTransmitterState = false;
    }  
  }  
  
  return result;
}

xComPortHandle xU1_SerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
        xComPortHandle xReturn = serHANDLE;
	/* Create the queues used to hold Rx and Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* If the queues were created correctly then setup the serial port 
	hardware. */
	if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
	{
		portENTER_CRITICAL();
		{
			/* Enable the USART clock. */
   			AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_US0 );

			AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, ( ( unsigned portLONG ) AT91C_PA5_RXD0 ) | ( ( unsigned portLONG ) AT91C_PA6_TXD0 ), serNO_PERIPHERAL_B_SETUP );

			/* Set the required protocol. */
			AT91F_US_Configure( serCOM0, configCPU_CLOCK_HZ, AT91C_US_ASYNC_MODE, ulWantedBaud, serNO_TIMEGUARD );

			/* Enable Rx and Tx. */
			serCOM0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;

			/* Enable the Rx interrupts.  The Tx interrupts are not enabled
			until there are characters to be transmitted. */
    		        AT91F_US_EnableIt( serCOM0, AT91C_US_RXRDY );

			/* Enable the interrupts in the AIC. */
			AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, AT91C_ID_US0, serINTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, ( void (*)( void ) ) vU1_SerialISREntry );
			AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_US0 );
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	/* This demo file only supports a single port but we have to return 
	something to comply with the standard demo header file. */
	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xU1_SerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xRxedChars != NULL )
        {
          if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
          {
                  return pdTRUE;
          }
        }
        
        return pdFALSE;
}
/*-----------------------------------------------------------*/

void vU1_SerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength )
{
        signed portCHAR *pxNext;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
        if( xCharsForTx != NULL )
        {  
          pxNext = ( signed portCHAR * ) pcString;
          
          for( int i = 0; i < usStringLength; i++ )
          {
            xQueueSendToBack( xCharsForTx, pxNext, serNO_BLOCK );
            pxNext++;
          }  

          bTransmitterState = false;
            
          vU1_InterruptOn();
        }
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xU1_SerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}
        
        bTransmitterState = false;
        
	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */
	vU1_InterruptOn();

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vU1_SerialClose( xComPortHandle xPort )
{
  /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

/* Serial port ISR.  This can cause a context switch so is not defined as a
standard ISR using the __irq keyword.  Instead a wrapper function is defined
within serialISR.s79 which in turn calls this function.  See the port
documentation on the FreeRTOS.org website for more information. */
__arm void vU1_SerialISR( void )
{
        unsigned portLONG ulStatus;
        signed portCHAR cChar;
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* What caused the interrupt? */
	ulStatus = serCOM0->US_CSR &= serCOM0->US_IMR;

	if( ulStatus & AT91C_US_TXRDY )
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			serCOM0->US_THR = cChar;
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			vU1_InterruptOff();
                        
                        bTransmitterState = true;
		}		
	}

	if( ulStatus & AT91C_US_RXRDY )
	{
		/* The interrupt was caused by a character being received.  Grab the
		character from the RHR and place it in the queue or received 
		characters. */
		cChar = serCOM0->US_RHR;
		xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
	}

	/* If a task was woken by either a character being received or a character 
	being transmitted then we may need to switch to another task. */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	/* End the interrupt in the AIC. */
	AT91C_BASE_AIC->AIC_EOICR = 0;
}




	
