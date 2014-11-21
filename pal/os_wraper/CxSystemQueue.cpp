#include "CxSystemQueue.h"

//------------------------------------------------------------------------------

CxQueue CxSystemQueue::sysQueue( 50, sizeof(TCommand) );

//------------------------------------------------------------------------------

CxSystemQueue::CxSystemQueue()
{

}

CxSystemQueue::~CxSystemQueue()
{

}

//------------------------------------------------------------------------------

bool CxSystemQueue::SendCommand( TCommand *pCommand )
{
  bool result = sysQueue.Send( reinterpret_cast<void*>(pCommand) ); 
  if( result == false ) 
  {
    return false;
  }    
  return result;  
}

bool CxSystemQueue::SendCommandToBack( TCommand *pCommand )
{
  bool result = sysQueue.SendToBack( reinterpret_cast<void*>(pCommand) ); 
  return result;  
}

bool CxSystemQueue::SendCommandToFront( TCommand *pCommand )
{
  bool result = sysQueue.SendToFront( reinterpret_cast<void*>(pCommand) ); 
  return result;
}

bool CxSystemQueue::ReceiveCommand( TCommand *pCommand )
{
  bool result = sysQueue.Receive( reinterpret_cast<void*>(pCommand) ); 
  if( result == false ) 
  {
    return false;
  }  
  return result;
}

bool CxSystemQueue::PeekCommand( TCommand *pCommand )
{
  bool result = sysQueue.Peek( reinterpret_cast<void*>(pCommand) ); 
  return result; 
}

unsigned long CxSystemQueue::IsCommand( )
{
  unsigned long result = sysQueue.Occupancy( ); 
  return result; 
}

void CxSystemQueue::LockQueu()
{

}

void CxSystemQueue::UnlockQueu()
{

}


