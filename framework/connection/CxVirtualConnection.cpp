#include "CxDebug.h"
#include "CxVirtualConnection.h"

//------------------------------------------------------------------------------

CxVirtualConnection::CxVirtualConnection( CxEvent::EventType EventOnInput, CxEvent::EventType EventOnOutput ):
  CxConnection( CxConnection::CtConnectionSerial )
{  
  ecb.RxEvent = EventOnInput;
  ecb.TxEvent = EventOnOutput;
}

int CxVirtualConnection::read(char *buffer, unsigned short size)
{
  int result = -1;
  if( (buffer != NULL) && ( ScOpen == GetConnectionState()) )
  {  
    result = Serial.ReadFromInputBuffer(buffer, size);
  }   
  return result;
}

int CxVirtualConnection::write(char *buffer, unsigned short size)
{
  int result = -1;
  if( (buffer != NULL) && ( ScOpen == GetConnectionState()) )
  {  
    Serial.LoadOutputBuffer(buffer, size); 
    Serial.StartTxD();
    result = 1;
  }   
  return result;
}

//------------------------------------------------------------------------------

void CxVirtualConnection::getState( pDCB pointerOnDCB )
{
  if( ScOpen == GetConnectionState() )
  {  
    Serial.GetCommState( pointerOnDCB ); 
  }   
}

bool CxVirtualConnection::setState( pDCB pointerOnDCB )
{
  bool result = false;
  if( ScOpen == GetConnectionState() )
  {  
    result = Serial.SetCommState( pointerOnDCB ); 
  }   
  return result;
}   

void CxVirtualConnection::getEvent( pECB pointerOnECB )
{
  if( ScOpen == GetConnectionState() )
  {  
    *pointerOnECB = ecb; 
  }  
}
  
//------------------------------------------------------------------------------

void CxVirtualConnection::process()
{
   if( true == Serial.ReceiveIsCompleted()   )  sendEvent( ecb.RxEvent, NULL ); 
   //if( true == Serial.TransitionIsCompleted() ) sendEvent( ecb.TxEvent, NULL ); 
}
