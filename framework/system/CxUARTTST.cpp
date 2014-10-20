#include "CxUARTTST.h"
#include "Utils.h"

//------------------------------------------------------------------------------

CxUARTTST::CxUARTTST( ):
   CxThreadIO( "UTST_TASK", "U1_DRV" )
  ,State( ST_U_UNKNOWN )
  ,testText(" uart test ")
{
    uDCB.BaudRate = 9600;   
    uDCB.Parity   = 0; 
    uDCB.ByteSize = 8;
    uDCB.StopBits = 1;
} 

CxUARTTST::~CxUARTTST( )
{

}

CxUARTTST &CxUARTTST::getInstance( )
{
  static CxUARTTST theInstance;
  return theInstance;
}

void CxUARTTST::Start()
{ 
  // create thread
  task_run( );
}

//------------------------------------------------------------------------------

void CxUARTTST::ThreadProcessor()
{
  switch( State )
  {    
      case ST_U_UNKNOWN : 
      {         
        State = ST_U_CONFIG;                                                    // put in next state
        break;
      }
      case ST_U_CONFIG :  
      {                 
        TCommand Command  = { threadID, drvID, request, C_SET_DCB, &uDCB };
        SendCommand( &Command );                 
        State = ST_U_NORMAL_WORK;                                               // put in next state
        break;
      }
      case ST_U_NORMAL_WORK :  
      {         
        COMDATA.packSize = 11;
        mod_strncpy(COMDATA.Data, testText, 50 );
        TCommand Command  = { threadID, drvID, request, C_SEND_DATA, &COMDATA };
        SendCommand( &Command ); 
        task_sleep( 100 );
        break;
      }	      

      default : break;
   }
}

//------------------------------------------------------------------------------

