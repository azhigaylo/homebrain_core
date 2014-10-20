#ifndef _CX_VIRTUAL_CONNECTION
#define _CX_VIRTUAL_CONNECTION

#include "CxConnection.h"
#include "CxUartBase.h"
#include "IxEventProducer.h"

//------------------------------------------------------------------------------

class CxVirtualConnection : public CxConnection, IxEventProducer
{
 public:
  
   CxVirtualConnection( CxEvent::EventType EventOnInput, CxEvent::EventType EventOnOutput );
   ~CxVirtualConnection(){}
   
   // for serial & file
   virtual int read (char *buffer, unsigned short size);
   virtual int write(char *buffer, unsigned short size);
   
   void getState( pDCB pointerOnDCB );
   bool setState( pDCB pointerOnDCB );
   
   void getEvent( pECB pointerOnECB );
      
 protected: 
   
   void process();
   
   CxUartBase Serial;        
   
 private:    
      
   ECB ecb;                                                     // pointer on serial interface

}; typedef CxVirtualConnection *pCxVirtualConnection;

#endif /*_CX_VIRTUAL_CONNECTION*/   
