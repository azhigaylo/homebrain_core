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
  __arm void vU2_SerialISR( void );
  void vTaskSwitchContext( void );
  void vU2_SerialISREntry( void );
#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------


xComPortHandle xU2_SerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength );

xComPortHandle xU2_SerialPortInit( eCOMPort ePort, eBaud eWantedBaud, eParity eWantedParity, eDataBits eWantedDataBits, eStopBits eWantedStopBits, unsigned portBASE_TYPE uxBufferLength );

void vU2_SerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength );

signed portBASE_TYPE xU2_SerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime );

signed portBASE_TYPE xU2_SerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime );

portBASE_TYPE xU2_SerialWaitForSemaphore( xComPortHandle xPort );

void vU2_SerialClose( xComPortHandle xPort );

bool vU2_isTransmissionFinished( xComPortHandle xPort );

#endif

