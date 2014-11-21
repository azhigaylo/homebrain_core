#ifndef _CX_QUEUE
#define _CX_QUEUE

//----------------------only wraper for FREERTOS xQueue ------------------------

#include "FreeRTOS.h"
#include "queue.h"

//------------------------------------------------------------------------------

class CxQueue
{ 
  public:       

    // function's   
    CxQueue( unsigned long queueLength, unsigned long itemSize );      
    ~CxQueue();

    bool Send           ( void *pItemToQueue   );   
    bool SendToBack     ( void *pItemToQueue   );
    bool SendToFront    ( void *pItemToQueue   );
    bool Receive        ( void *pItemFromQueue );
    bool Peek           ( void *pItemFromQueue );
    bool AddToRegistry  ( signed char *Name    );
    unsigned long Occupancy ( );

  protected:  

    void Create ( unsigned long queueLength, unsigned long itemSize );    
    void Delete ( );
    
  private:       
    
    xQueueHandle xQueue;
    
   CxQueue(const CxQueue& rhs);
   CxQueue& operator=(const CxQueue& rhs);  
   
 }; typedef CxQueue *pCxQueue;

//------------------------------------------------------------------------------

#endif // _IX_QUEUE

