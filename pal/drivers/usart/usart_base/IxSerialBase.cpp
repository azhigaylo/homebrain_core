#include "..\IxSerialBase.h"

//----------------interface class for serial communication----------------------

IxSerialBase::IxSerialBase()
{ 
}

void IxSerialBase::GetCommState( pDCB pointerOnDCB )
{
  GetInternalCommState( pointerOnDCB );
}  

bool IxSerialBase::SetCommState( pDCB pointerOnDCB )
{
  return SetInternalCommState( pointerOnDCB );
}

void IxSerialBase::GetInternalCommState( pDCB pointerOnDCB )
{
  *pointerOnDCB = internal_dcb;
}

bool IxSerialBase::SetInternalCommState( pDCB pointerOnDCB )
{
   if( (pointerOnDCB->BaudRate >= 1200) && (pointerOnDCB->BaudRate <= 115200) )
   if( pointerOnDCB->Parity <= 4 )  
   if( (pointerOnDCB->ByteSize >= 4) && (pointerOnDCB->ByteSize <= 8) )
   if( pointerOnDCB->StopBits <= 2 )
   {
      internal_dcb.BaudRate = pointerOnDCB->BaudRate;
      internal_dcb.Parity   = pointerOnDCB->Parity;
      internal_dcb.ByteSize = pointerOnDCB->ByteSize;
      internal_dcb.StopBits = pointerOnDCB->StopBits;
      return true;  
   }
   return false;
}

