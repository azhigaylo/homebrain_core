//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "pio.h"

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif
  __arm void vPIO_SerialISR( void );
  void vPIO_SerialISREntry( void );
  void vTaskSwitchContext( void );  
#ifdef __cplusplus
}
#endif


extern void PIO_InitializeInterrupts(unsigned int priority);

extern void PIO_EnableIt(const Pin *pPin);

extern void PIO_DisableIt(const Pin *pPin);

