#ifndef SERIAL_X_COMMS_H
#define SERIAL_X_COMMS_H

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/* Misc constants. */
#define serINVALID_QUEUE				( ( xQueueHandle )   0 )
#define serHANDLE					( ( xComPortHandle ) 1 )
#define serNO_BLOCK					( ( portTickType )   0 )
#define serNO_TIMEGUARD					( ( unsigned portLONG ) 0 )
#define serNO_PERIPHERAL_B_SETUP		        ( ( unsigned portLONG ) 0 )

//------------------------------------------------------------------------------

typedef void * xComPortHandle;

typedef enum
{ 
	serCOM1, 
	serCOM2, 
	serCOM3, 
	serCOM4, 
	serCOM5, 
	serCOM6, 
	serCOM7, 
	serCOM8 
} eCOMPort;

typedef enum 
{ 
	serNO_PARITY, 
	serODD_PARITY, 
	serEVEN_PARITY, 
	serMARK_PARITY, 
	serSPACE_PARITY 
} eParity;

typedef enum 
{ 
	serSTOP_1, 
	serSTOP_2 
} eStopBits;

typedef enum 
{ 
	serBITS_5, 
	serBITS_6, 
	serBITS_7, 
	serBITS_8 
} eDataBits;

typedef enum 
{ 
	ser50,		
	ser75,		
	ser110,		
	ser134,		
	ser150,    
	ser200,
	ser300,		
	ser600,		
	ser1200,	
	ser1800,	
	ser2400,   
	ser4800,
	ser9600,		
	ser19200,	
	ser38400,	
	ser57600,	
	ser115200
} eBaud;

#endif

