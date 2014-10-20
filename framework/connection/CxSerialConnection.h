#ifndef _CX_SERIAL_CONNECTION
#define _CX_SERIAL_CONNECTION

#include "CxConnection.h"
#include "CxThreadIO.h"
#include "..\eventpool\IxEventProducer.h"

//------------------------------------------------------------------------------

struct DCB
{
  unsigned long BaudRate;                                                       // current baud rate 
  char Parity;                                                                  // 0-4=no,odd,even,mark,space 
  char ByteSize;                                                                // number of bits/byte, 4-8 
  char StopBits;                                                                // 0,1,2 = 1, 1.5, 2      
}; 
typedef DCB *pDCB;

struct ECB
{
   CxEvent::EventType RxEvent;
   CxEvent::EventType TxEvent;   
}; 
typedef ECB *pECB;
 
//------------------------------------------------------------------------------

class CxSerialConnection : public CxConnection, public CxThreadIO, public IxEventProducer
{
 public:

   enum TSerialDrvCommand
   {
      C_SET_DCB = 1,
      C_SET_IN_BUFFER,
      C_SET_OUT_BUFFER,
      C_SEND_DATA,
      C_NEED_DATA_PROCESS,
      C_GET_IO_SIZE,         
      C_SEND_DATA_END,
      C_STOP
   };
   
   enum TState
   {
      ST_U_UNKNOWN = 0,
      ST_U_SET_DCB,
      ST_U_SET_INP_BUFFER,
      ST_U_SET_OUT_BUFFER,
      ST_U_NORMAL_WORK
   };   
      
   CxSerialConnection( char * connectionName, char * driverName,
                       CxEvent::EventType EventOnInput, CxEvent::EventType EventOnOutput 
                     );
   ~CxSerialConnection();

   // for serial & file
   virtual int read ( int, void*, unsigned short );
   virtual int write( int, const void*, unsigned short );
   
   void getState( pDCB pointerOnDCB );
   bool setState( pDCB pointerOnDCB );
   
   void getEvent( pECB pointerOnECB );
   
   void StartConnection();
      
 protected: 
   
   virtual void ThreadProcessor();
   virtual void CommandProcessor( TCommand &Command );
   
 private:    
   
   TState State;                                                                // state of the system
   
   ECB ecb;                                                                     // event block
      
   // used for "C_SET_DCB"
   DCB dcb;
   
   // used for "C_SET_IN_BUFFER"
   char* pInputBuffer;                                                          // pointer for input buffer
   // used for "C_SET_OUT_BUFFER"
   char* pOutputBuffer;                                                         // pointer for output buffer   
   // used for "C_SEND_DATA"
   unsigned short OutputDataLength;                                             // size for input buffer 
   // used for "C_NEED_DATA_PROCESS"
   unsigned short InputDataLength;                                              // size for output buffer    
   // used for "C_GET_IO_SIZE"
   unsigned short SizeofIObuffer;

}; typedef CxSerialConnection *pCxSerialConnection;

#endif /*_CX_SERIAL_CONNECTION*/   
