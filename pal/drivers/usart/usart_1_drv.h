#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

#include "FreeRTOS.h"
#include "usart_x_drv.h"
#include "queue.h"

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
  __arm void vU1_SerialISR( void );
  void vTaskSwitchContext( void );
  void vU1_SerialISREntry( void );
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------

xComPortHandle xU1_SerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength );

xComPortHandle xU1_SerialPortInit( eCOMPort ePort, eBaud eWantedBaud, eParity eWantedParity, eDataBits eWantedDataBits, eStopBits eWantedStopBits, unsigned portBASE_TYPE uxBufferLength );

void vU1_SerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength );

signed portBASE_TYPE xU1_SerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime );

signed portBASE_TYPE xU1_SerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );

portBASE_TYPE xU1_SerialWaitForSemaphore( xComPortHandle xPort );

void vU1_SerialClose( xComPortHandle xPort );

bool vU1_isTransmissionFinished( xComPortHandle xPort );

#endif

