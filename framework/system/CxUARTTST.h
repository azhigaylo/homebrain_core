#ifndef _CX_UARTTST
#define _CX_UARTTST

//------------------------------------------------------------------------------
#include "CxThreadIO.h"
//------------------------------------------------------------------------------

class CxUARTTST : public CxThreadIO
{   

   struct TCOMDATA
   {
      unsigned short packSize;
      char Data[50];
   }; typedef TCOMDATA *pTCOMDATA;     
   
   struct DCB
   {
    unsigned long BaudRate;                                                        // current baud rate 
    char Parity;                                                                   // 0-4=no,odd,even,mark,space 
    char ByteSize;                                                                 // number of bits/byte, 4-8 
    char StopBits;                                                                 // 0,1,2 = 1, 1.5, 2      
   }; typedef DCB *pDCB;

   enum TLauncherState
   {
      C_SET_DCB = 1,
      C_SEND_DATA,
      C_NEED_DATA_PROCESS,
      C_STOP, 
      C_GET_IO_SIZE      
   };

   enum TState
   {
      ST_U_UNKNOWN = 0,
      ST_U_CONFIG,
      ST_U_NORMAL_WORK
   };
  
  public:  

     // function's   
     CxUARTTST();       
     ~CxUARTTST();
     
     static CxUARTTST& getInstance();
     
     // start all parts of system task
     void Start();

  protected:    
    
     virtual void ThreadProcessor();
     virtual void CommandProcessor( TCommand &Command ){}

  private:       
     // FSM process
     void Process();    
     
     TCOMDATA COMDATA;
     TState State;
     DCB uDCB;
     
     char *testText;
       
 }; typedef CxUARTTST *pCxUARTTST;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

