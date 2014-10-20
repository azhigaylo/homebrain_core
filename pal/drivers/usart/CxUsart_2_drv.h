#ifndef _CX_USART_2_DRV
#define _CX_USART_2_DRV

//------------------------------------------------------------------------------

#include "IxDriver.h"
#include "IxSerialBase.h"
#include "usart_2_drv.h"

//------------------------------------------------------------------------------

class CxUsart_2_drv : public IxDriver, public IxSerialBase
{
  public:  
     
     static CxUsart_2_drv& getInstance();

  protected:

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

  protected:
     
   // load Tx / read Rx port's buffer    
   virtual void LoadOutputBuffer( char *buf, const unsigned short buf_size );     
   virtual unsigned short ReadFromInputBuffer( char *buf, const unsigned short buf_limit );    
   
   // skip "recive package" flag and his size
   virtual void FlushInputBuffers();         
   virtual void FlushOutputBuffers();      
   
   // check complite trancmite / recieve of package
   virtual bool IsTransitionCompleted();                   
   virtual bool IsReceiveCompleted();      
   
   // metod for start / stop trancmitte data from Tx buffer in port
   virtual void StartTxD(){ IO_UART.cOPack_trans = true; }
   virtual void StopTxD(){}
   
   // metod for blocking recieve data to Rx buffer from port
   virtual void StopRxD(){}

   virtual unsigned short GetSizeOfInputBuffer() const;
   virtual unsigned short GetSizeOfOutputBuffer()const;
   
   // new items   
   virtual bool SetCommState( pDCB pointerOnDCB ); 
   
   // it's very critical function - please use it with care.
   TIO_UART & getCommunicationBlock( ){ return IO_UART; }

   // reinit 
   virtual void reinit(); 

  private:       
    
    // function's   
    CxUsart_2_drv();       
    ~CxUsart_2_drv();
     
    virtual void CommandProcessor( TCommand &Command );
    virtual void ThreadProcessor ( );
    
    xComPortHandle comPortHandle;                                               // COM port handle

    unsigned short timeOut;                                                     // time out for package receive in mS  
    unsigned long lCounter;                                                     // counter for time out processing 
    
    // used for "C_GET_IO_SIZE" command
    unsigned short inputBufferSize;                                             // IO buffer size
    unsigned short outputBufferSize;
    
    // used for processing "C_SET_IN_BUFFER" and "C_SET_OUT_BUFFER" 
    char* pInputBuffer;                                                         // pointer on IO buffer in top level serial connection  
    char* pOutputBuffer; 

    // used for "C_NEED_DATA_PROCESS"
    unsigned short InputLength;  
    
    // communication data block    
    TIO_UART IO_UART;

 }; typedef CxUsart_2_drv *pCxUsart_2_drv;
 
//------------------------------------------------------------------------------

#endif // _CX_USART_2_DRV

