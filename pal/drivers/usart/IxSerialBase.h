#ifndef _IX_SERIAL_BASE
#define _IX_SERIAL_BASE

//------------------------------------------------------------------------------

#define SizeOfInputBuffer  50
#define SizeOfoutputBuffer 50

//----------------interface class for serial communication----------------------

enum TPortParity
{
  // 0-4=no,odd,even,mark,space 
  no_parity = 0,  
  odd_parity,
  even_parity,
  mark_parity,
  space_parity
};

struct DCB
{
  unsigned long BaudRate;                                                        // current baud rate 
  char Parity;                                                                   // 0-4=no,odd,even,mark,space 
  char ByteSize;                                                                 // number of bits/byte, 4-8 
  char StopBits;                                                                 // 0,1,2 = 1, 1.5, 2      
}; typedef DCB *pDCB;

struct TIO_UART
{
   char cOPack_trans;
   unsigned long  lOPack_numb;
   unsigned short sOPack_size;
   char aOPack[SizeOfoutputBuffer];
   char cIPack_rec;
   unsigned long  lIPack_numb;
   unsigned short sIPack_size;
   char aIPack[SizeOfInputBuffer];
}; typedef TIO_UART *pTIO_UART;

class IxSerialBase
{
 public: 
   
   IxSerialBase();
   ~IxSerialBase(){}

   // load Tx / read Rx port's buffer 
   virtual void LoadOutputBuffer( char *buf, const unsigned short buf_size ) = 0;     
   virtual unsigned short ReadFromInputBuffer( char *buf, const unsigned short buf_limit )= 0;    
   
   // skip "recive package" flag and his size   
   virtual void FlushInputBuffers()  = 0;    
   virtual void FlushOutputBuffers() = 0; 
   
   // check complite trancmite / recieve of package                
   virtual bool IsReceiveCompleted()    = 0;    
   virtual bool IsTransitionCompleted() = 0;        

   // metod for start / stop trancmitte data from Tx buffer in port
   virtual void StartTxD() = 0;
   virtual void StopTxD()  = 0;
   
   // metod for blocking recieve data to Rx buffer from port
   virtual void StopRxD()  = 0;

   // new items   
   virtual bool SetCommState( pDCB pointerOnDCB ); 
           void GetCommState( pDCB pointerOnDCB );    
   
   // size of buffers
   virtual unsigned short GetSizeOfInputBuffer() const = 0;
   virtual unsigned short GetSizeOfOutputBuffer()const = 0;
   
 protected:   
   
           void GetInternalCommState( pDCB pointerOnDCB ); 
           bool SetInternalCommState( pDCB pointerOnDCB );    
   
 private:    

   DCB internal_dcb;   
   
}; typedef IxSerialBase *pIxSerialBase;

#endif /*_IX_SERIAL_BASE*/   
