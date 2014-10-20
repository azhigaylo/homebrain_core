//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "pio_it.h"
#include "pio.h"
#include "aic.h"
#include <board.h>
#include "FreeRTOS.h"

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

/// Maximum number of interrupt sources that can be defined.
#define MAX_INTERRUPT_SOURCES       2

//------------------------------------------------------------------------------
//         Local types
//------------------------------------------------------------------------------

/// Describes a PIO interrupt source, including the PIO instance triggering the
/// interrupt and the associated interrupt handler.
typedef struct _InterruptSource 
{
    /// Interrupt source pin.
    const Pin *pPin;

} InterruptSource;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// List of interrupt sources.
static InterruptSource pSources;

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Handles all interrupts on the given PIO controller.
/// \param id  PIO controller ID.
/// \param pBase  PIO controller base address.
//------------------------------------------------------------------------------
void PioInterruptHandler( )
{
    unsigned int status;
    
    // Check PIO controller status
    status = AT91C_BASE_PIOA->PIO_ISR;
    status &= AT91C_BASE_PIOA->PIO_IMR;
       
    if (status != 0) 
    {
       // Source has PIOs which have changed
       if ((status & pSources.pPin->mask) != 0) 
       {
          status &= ~(pSources.pPin->mask);
       }
    }
}

//------------------------------------------------------------------------------

__arm void vPIO_SerialISR( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    PioInterruptHandler( );
        
    // if a task was woken by either a character being received or a character 
    // being transmitted then we may need to switch to another task. 
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    
    // End the interrupt in the AIC. 
    AT91C_BASE_AIC->AIC_EOICR = 0;
  
}  


//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the PIO interrupt management logic.
/// \param priority  PIO controller interrupts priority.
//------------------------------------------------------------------------------
void PIO_InitializeInterrupts(unsigned int priority)
{
    // Configure PIO interrupt sources
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;
    AT91C_BASE_PIOA->PIO_ISR;
    AT91C_BASE_PIOA->PIO_IDR = 0xFFFFFFFF;
    AIC_ConfigureIT(AT91C_ID_PIOA, priority, vPIO_SerialISREntry);
    AIC_EnableIT(AT91C_ID_PIOA);
}

//------------------------------------------------------------------------------
/// Enables the given interrupt source if it has been configured.
/// \param pPin  Interrupt source to enable.
//------------------------------------------------------------------------------
void PIO_EnableIt(const Pin *pPin)
{
    pPin->pio->PIO_ISR;
    pPin->pio->PIO_IER = pPin->mask;
}

//------------------------------------------------------------------------------
/// Disables a given interrupt source.
/// \param pPin  Interrupt source to disable.
//------------------------------------------------------------------------------
void PIO_DisableIt(const Pin *pPin)
{
    pPin->pio->PIO_IDR = pPin->mask;
}

