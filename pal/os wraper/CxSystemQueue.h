#ifndef _CX_SYSQUEUE
#define _CX_SYSQUEUE

//----------------------only wraper for FREERTOS xQueue ------------------------

#include "FreeRTOS.h"
#include "CxQueue.h"
#include "CxMutex.h"
//------------------------------------------------------------------------------

// command type
#define identification_request        1
#define identification_response       2
#define request                       3
#define response                      4

// common command ID
#define DIReq         1
#define DIRes         2

//------------------------------------------------------------------------------


struct TCommand
{
  unsigned short SenderID;  
  unsigned short ConsumerID;  
  unsigned char  ComType;
  unsigned char  ComID;
  void *Container;
};

class CxSystemQueue
{ 
  public:       

    // function's       
    ~CxSystemQueue();

    bool SendCommand        ( TCommand *pCommand );   
    bool SendCommandToBack  ( TCommand *pCommand );
    bool SendCommandToFront ( TCommand *pCommand );
    bool ReceiveCommand     ( TCommand *pCommand );
    bool PeekCommand        ( TCommand *pCommand );
    unsigned long IsCommand ( );
    
    void LockQueu();
    void UnlockQueu();

  protected: 

    CxSystemQueue( );  

    void Create ( unsigned long queueLength, unsigned long itemSize );    
    void Delete ( );
    
  private:       

   static CxQueue sysQueue;

   CxSystemQueue(const CxSystemQueue& rhs);
   CxSystemQueue& operator=(const CxSystemQueue& rhs);    
      
 }; typedef CxSystemQueue *pCxSystemQueue;

//------------------------------------------------------------------------------

#endif // _CX_SYSQUEUE

