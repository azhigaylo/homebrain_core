#ifndef I2C_SERIAL_COMMS_H
#define I2C_SERIAL_COMMS_H

#include "FreeRTOS.h"
#include "queue.h"

//------------------------------------------------------------------------------

#define serINVALID_QUEUE				( ( xQueueHandle )   0 )
#define serNO_BLOCK					( ( portTickType )   0 )


//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
  __arm void vI2C_SerialISR( void );
  void vTaskSwitchContext( void );
  void vI2C_SerialISREntry( void );
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------

bool vI2C_SerialPortInit( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength );

void vI2C_SerialPutString( const signed portCHAR * const pcString, unsigned portSHORT usStringLength );

signed portBASE_TYPE vI2C_SerialGetChar( signed portCHAR *pcRxedChar, portTickType xBlockTime );

signed portBASE_TYPE vI2C_SerialPutChar( signed portCHAR cOutChar, portTickType xBlockTime );

void vI2C_SerialClose( );

bool vI2C_isTransmissionFinished( );

unsigned char TWI_WriteMultiple( AT91S_TWI *pTwi, unsigned char address, unsigned int iaddress,
                                      unsigned char isize, unsigned char *pData, unsigned int num );
#endif

