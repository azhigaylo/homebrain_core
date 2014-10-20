#include "CxSerialConnection.h"
#include "..\staticpool\CxStaticPool.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxSerialConnection::CxSerialConnection( char * connectionName, char * driverName, CxEvent::EventType EventOnInput, CxEvent::EventType EventOnOutput ):
   CxConnection ( CxConnection::CtConnectionSerial )
  ,CxThreadIO   ( connectionName, driverName )  
  ,State        ( ST_U_UNKNOWN ) 
  ,pInputBuffer ( NULL )  
  ,pOutputBuffer( NULL )  
  ,OutputDataLength ( 0  )
  ,InputDataLength  ( 0  )  
  ,SizeofIObuffer   ( 50 )
{  
 
  // event setup
  ecb.RxEvent = EventOnInput;
  ecb.TxEvent = EventOnOutput;
  
  // port congiguration
  dcb.BaudRate = 9600;   
  dcb.Parity   = 0; 
  dcb.ByteSize = 8;
  dcb.StopBits = 1;
  
  // create uart IO buffers
  CxStaticPool &StaticPool = CxStaticPool::getInstance(); 
  pInputBuffer  = reinterpret_cast<char*>( StaticPool.pStaticMalloc(SizeofIObuffer) );    
  pOutputBuffer = reinterpret_cast<char*>( StaticPool.pStaticMalloc(SizeofIObuffer) );    
  
  if( pInputBuffer == NULL || pOutputBuffer == NULL )
  {
    // vax vax beda !!!! pamyati kuuuu....
    // we should never come to this place
    deactivate();        
  }
  else
  {  
    // activate connection  
    activate();
  }  
}

CxSerialConnection::~CxSerialConnection()
{
  deactivate( );
}

int CxSerialConnection::read( int fileID, void* buffer, unsigned short size )
 
{
  int result = -1;

  if( (buffer != NULL) && ( ScOpen == GetConnectionState()) )
  {  
    if( true == mod_memcpy( reinterpret_cast<char*>(buffer), pInputBuffer, InputDataLength, size ) )
    {
      result = InputDataLength;      
    }
    else
    {
      result = size;
    }  
  }   
  
  return result;
}

int CxSerialConnection::write( int fileID, const void* buffer, unsigned short size )
 
{
  int result = -1;

  if( (pOutputBuffer != NULL) && (buffer != NULL) && (ScOpen == GetConnectionState()) )
  {  
    // set data for driver
    mod_memcpy( pOutputBuffer, (char*)(buffer), size, SizeofIObuffer );
    // set output data length
    OutputDataLength = size;      
    TCommand Command  = { threadID, drvID, request, C_SEND_DATA, &OutputDataLength };
    SendCommand( &Command );    
    result = 1;
  }  

  return result;
}

//------------------------------------------------------------------------------

void CxSerialConnection::getState( pDCB pointerOnDCB )
{
  if( ScOpen == GetConnectionState() )
  {  
    *pointerOnDCB = dcb; 
  }   
}

bool CxSerialConnection::setState( pDCB pointerOnDCB )
{
  bool result = false;
 
  if( ScOpen == GetConnectionState() )
  {  
    dcb = *pointerOnDCB; 

    TCommand Command  = { threadID, drvID, request, C_SET_DCB, &dcb };
    SendCommand( &Command );    

    result = true;
  }
  
  return result;
}   

void CxSerialConnection::getEvent( pECB pointerOnECB )
{
  if( ScOpen == GetConnectionState() )
  {  
    *pointerOnECB = ecb; 
  }   
}
  
void CxSerialConnection::StartConnection()
{    
  DBG_SCOPE(CxSerialConnection, CxSerialConnection)
  
  if( pInputBuffer == NULL || pOutputBuffer == NULL )
  {
    // !!!     
  }
  else
  {  
    DBG_MSG( ("[M] connection - %s was started \n\r", pcTaskName) );   
    // create thread
    task_run( );      
  }    
}
//------------------------------------------------------------------------------

void CxSerialConnection::ThreadProcessor()
{
  switch( State )
  {    
      case ST_U_UNKNOWN : 
      {         
        State = ST_U_SET_DCB;                                                    // put in next state
        break;
      }
      case ST_U_SET_DCB :  
      {                 
        TCommand Command  = { threadID, drvID, request, C_SET_DCB, &dcb };
        SendCommand( &Command );                 
        State = ST_U_SET_INP_BUFFER;                                            // put in next state
        break;
      }
      case ST_U_SET_INP_BUFFER :  
      {                 
        TCommand Command  = { threadID, drvID, request, C_SET_IN_BUFFER, &pInputBuffer };
        SendCommand( &Command );                 
        State = ST_U_SET_OUT_BUFFER;                                            // put in next state
        break;
      }
      case ST_U_SET_OUT_BUFFER :  
      {                 
        TCommand Command  = { threadID, drvID, request, C_SET_OUT_BUFFER, &pOutputBuffer };
        SendCommand( &Command );                 
        State = ST_U_NORMAL_WORK;                                               // put in next state
        break;
      }      
      case ST_U_NORMAL_WORK :  
      {         
        
        break;
      }	      

      default : break;
   }    
}

void CxSerialConnection::CommandProcessor( TCommand &Command )
{
  switch( Command.ComID )
  {    
      case C_NEED_DATA_PROCESS : 
      {         
        InputDataLength = *(reinterpret_cast<unsigned short*>( Command.Container ));
        sendEvent( ecb.RxEvent, NULL );
        break;
      }    
      case C_SEND_DATA_END : 
      {         
        sendEvent( ecb.TxEvent, NULL );
        break;
      }          
      default : break;
  }  
}
