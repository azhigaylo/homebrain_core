#ifndef _CX_QUEUE
#define _CX_QUEUE

//------------------------------------------------------------------------------
#include "ptypes.h"
#include <fcntl.h>    // Defines O_* constants
#include <sys/stat.h> // Defines mode constants
#include <mqueue.h>
//------------------------------------------------------------------------------
#define configMAX_QUEUE_NAME_LEN 50
//------------------------------------------------------------------------------

class CxQueue
{
  public:

    // function's
    CxQueue ( const char *name, int32_t queueLength, int32_t itemSize );
    ~CxQueue();

    bool send       (const void *pItemToQueue, int32_t msg_size);
    bool timedSend  (const void *pItemToQueue, int32_t msg_size, uint64_t time);
    int32_t receive (void *pItemFromQueue, int32_t msg_size);
    int32_t occupancy( );

  private:

   mqd_t   xQueue;
   mq_attr queueAttr;
   char queueName[configMAX_QUEUE_NAME_LEN];

   CxQueue(const CxQueue& rhs){}
   CxQueue& operator=(const CxQueue& rhs){}

 }; typedef CxQueue *pCxQueue;

//------------------------------------------------------------------------------

#endif // _IX_QUEUE

