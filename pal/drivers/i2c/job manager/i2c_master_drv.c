//------------------------------------------------------------------------------
#include <stdlib.h>
#include "i2c_master_drv.h"
#include "twi\twi.h"
#include "at91sam7s128\lib_AT91SAM7S.h"


//------------------------------------------------------------------------------

// Interrupt control macros. 
#define serINTERRUPT_LEVEL				( 5 )
#define vI2C_InterruptOn()				TWI_EnableIt (i2cCOM, AT91C_TWI_TXRDY|AT91C_TWI_RXRDY|AT91C_TWI_NACK);
#define vI2C_InterruptOff()				TWI_DisableIt(i2cCOM, AT91C_TWI_TXRDY|AT91C_TWI_RXRDY|AT91C_TWI_NACK);

// Queues used to hold received characters, and characters waiting to be transmitted. 

static xQueueHandle xRxedChars  = NULL; 
static xQueueHandle xCharsForTx = NULL; 

static bool bTransmitterState   = false;   // true - transmission finished / false - transmission in process

//------------------------------------------------------------------------------

bool vI2C_isTransmissionFinished( )
{
    bool result = false;
    
    result = bTransmitterState;
    
    if( true == result )
    {
      bTransmitterState = false;
    }  
    
    return result;
}

bool vI2C_SerialPortInit( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
    bool xReturn = false;
    // Create the queues used to hold Rx and Tx characters. 
    xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
    xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

    // If the queues were created correctly then setup the serial port hardware.
    if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
    {
            portENTER_CRITICAL();
            {
                AT91F_TWI_Open( ulWantedBaud );

                // Enable the interrupts in the AIC. 
                AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, AT91C_ID_TWI, serINTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, ( void (*)( void ) ) vI2C_SerialISREntry );
                AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_TWI );
            }
            portEXIT_CRITICAL();
            
            xReturn = true;
    }

    // This demo file only supports a single port but we have to return 
    // something to comply with the standard demo header file. 
    return xReturn;
}

//------------------------------------------------------------------------------

void vI2C_SerialPutString( const signed portCHAR * const pcString, unsigned portSHORT usStringLength )
{
    signed portCHAR *pxNext;
    
    // Send each character in the string, one at a time. 
    if( xCharsForTx != NULL )
    {  
      pxNext = ( signed portCHAR * ) pcString;
      
      for( int i = 0; i < usStringLength; i++ )
      {
        xQueueSendToBack( xCharsForTx, pxNext, serNO_BLOCK );
        pxNext++;
      }  
    
      bTransmitterState = false;
        
      vI2C_InterruptOn();
    }
}

//------------------------------------------------------------------------------

signed portBASE_TYPE vI2C_SerialGetChar( signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
    // Get the next character from the buffer.  Return false if no characters
    // are available, or arrive before xBlockTime expires. 
    if( xRxedChars != NULL )
    {
      if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
      {
              return pdTRUE;
      }
    }
    
    return pdFALSE;
}

//------------------------------------------------------------------------------

signed portBASE_TYPE vI2C_SerialPutChar( signed portCHAR cOutChar, portTickType xBlockTime )
{
    // Place the character in the queue of characters to be transmitted. 
    if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
    {
            return pdFAIL;
    }
    
    bTransmitterState = false;
    
    // Turn on the Tx interrupt so the ISR will remove the character from the
    // queue and send it.   This does not need to be in a critical section as
    // if the interrupt has already removed the character the next interrupt
    // will simply turn off the Tx interrupt again. 
    vI2C_InterruptOn();
    
    return pdPASS;
}

//------------------------------------------------------------------------------

void vI2C_SerialClose( )
{
    // Not supported as not required by the demo application. 
}

//------------------------------------------------------------------------------

// Serial port ISR.  This can cause a context switch so is not defined as a
// standard ISR using the __irq keyword.  Instead a wrapper function is defined
// within i2c_master_ISR.s79 which in turn calls this function.

__arm void vI2C_SerialISR( void )
{
    unsigned portLONG ulStatus;
    signed portCHAR cChar;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    // What caused the interrupt? 
    ulStatus = i2cCOM->TWI_SR &= i2cCOM->TWI_IMR;
    
    if( ulStatus & AT91C_TWI_NACK )
    {
       /// vax vax kak plexo !!!!
       bTransmitterState = true;

       // if a task was woken by either a character being received or a character 
       // being transmitted then we may need to switch to another task. 
       portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
       
       // End the interrupt in the AIC. 
       AT91C_BASE_AIC->AIC_EOICR = 0;
       
       return;
    }  
    
    if( ulStatus & AT91C_TWI_TXRDY )
    {     
            // The interrupt was caused by the THR becoming empty.  Are there any
            // more characters to transmit? 
            if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
            {
                    // A character was retrieved from the queue so can be sent to the
                    // THR now. 
                    i2cCOM->TWI_THR = cChar;
            }
            else
            {
                    // Queue empty, nothing to send so turn off the Tx interrupt. 
                    vI2C_InterruptOff();
                    
                    bTransmitterState = true;
            }		  
    }
    
    if( ulStatus & AT91C_TWI_RXRDY )
    {
            // The interrupt was caused by a character being received.  Grab the
            // character from the RHR and place it in the queue or received 
            // characters. 
            cChar = i2cCOM->TWI_RHR;
            xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );     
    }
        
    // if a task was woken by either a character being received or a character 
    // being transmitted then we may need to switch to another task. 
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    
    // End the interrupt in the AIC. 
    AT91C_BASE_AIC->AIC_EOICR = 0;

}


//------------------------------------------------------------------------------

/// Asynchronously sends data to a slave on the TWI bus. An optional callback
/// function is invoked whenever the transfer is complete.
/// \param pTwid  Pointer to a Twid instance.
/// \param address  Slave address.
/// \param iaddress  Optional slave internal address.
/// \param isize  Number of internal address bytes.
/// \param pData  Data buffer to send.
/// \param num  Number of bytes to send.
/// \param pAsync  Pointer to an Asynchronous transfer descriptor.

//------------------------------------------------------------------------------

unsigned char TWI_WriteMultiple( AT91S_TWI *pTwi, unsigned char address, unsigned int iaddress,
                                      unsigned char isize, unsigned char *pData, unsigned int num )
{
    // Enable write interrupt and start the transfer
    TWI_StartWrite( pTwi, address, iaddress, isize ); 
    vI2C_SerialPutString( pData, num );
    
    return 0;
}
