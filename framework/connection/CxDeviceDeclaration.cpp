
#include "CxSerialConnection.h"
#include "CxFileSystemConnection.h"

// HW COM X
CxSerialConnection      Connection_1( "U1_CONN", "U1_DRV", CxEvent::EVENT_USART1_Rx, CxEvent::EVENT_USART1_Tx );      
CxSerialConnection      Connection_2( "U2_CONN", "U2_DRV", CxEvent::EVENT_USART2_Rx, CxEvent::EVENT_USART2_Tx );
CxFileSystemConnection  Connection_3( "F1_CONN", "SD_DRV" );
