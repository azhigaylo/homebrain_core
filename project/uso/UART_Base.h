#ifndef _UART_BASE
#define _UART_BASE
//define possible UART mode
#define even  (0x01)
#define none  (0x02)
//mode UART definition
#define UNUSE (0x00)
#define USER  (0x01)
#define USO   (0x02)
#define HOST  (0x03)
#define TERM  (0x04)

//--------------------------------------------------------------------------------------------
//-------------------------------BASE UART class----------------------------------------------
//--------------------------------------------------------------------------------------------
class CUART_BASE{
 public: 
   CUART_BASE(){
     I_BUF_SIZE = sizeof(IO_UART.aIPack);
     O_BUF_SIZE = sizeof(IO_UART.aOPack);
     MODE = UNUSE;
   }
   ~CUART_BASE(){}
   
   volatile TIO_UART IO_UART;
   volatile TCOM_SETUP COM_SETUP;
   char MODE;
      
   __monitor void LoadIO_Buf(unsigned char *buf, unsigned short buf_size){    
                  memcpy((char*)IO_UART.aOPack, buf, buf_size); 
                  IO_UART.sOPack_size = buf_size;  
                 }      
   __monitor unsigned short ReadIO_Buf(unsigned char *buf, unsigned short buf_size){
                   if(IO_UART.cIPack_rec){                   
                     unsigned short t_size = IO_UART.sIPack_size;
                     if(IO_UART.sIPack_size > buf_size){memcpy(buf, (char*)IO_UART.aIPack, buf_size);}
                      else{memcpy(buf, (char*)IO_UART.aIPack, IO_UART.sIPack_size);} 
                     IO_UART.cIPack_rec = false; IO_UART.sIPack_size = 0;
                     return(t_size);        
                   }else{return(0);}                                                              
                 }       
   __monitor void RecBufSkip(){
                   if(IO_UART.cIPack_rec){IO_UART.cIPack_rec = false; IO_UART.sIPack_size = 0;}
                 }                  
                 
   // virtual metod
   virtual __monitor void StartTxD(){}
   virtual __monitor void StopTxD(){}
   void SetMODE(char mode){MODE = mode;}  

   unsigned short I_BUF_SIZE;   
   unsigned short O_BUF_SIZE;   
}; typedef CUART_BASE *CUART_BASE_ptr;
#endif /*_UART_1_SLAVE*/   
