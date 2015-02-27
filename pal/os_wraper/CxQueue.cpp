//------------------------------------------------------------------------------
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
//------------------------------------------------------------------------------
#include "slog.h"
#include "utils.h"
#include "CxQueue.h"
//------------------------------------------------------------------------------
#define MSG_PRIO 1
//------------------------------------------------------------------------------

CxQueue::CxQueue( const char *name, int32_t queueLength, int32_t itemSize, bool blocked ):
    xQueue( -1 )
{
   queueName[0] = '/';
   strncpy_m( &queueName[1], const_cast<char*>(name), configMAX_QUEUE_NAME_LEN-1 );

   if (true == blocked)
   {
      queueAttr.mq_flags   = 0;            // Flags: 0 or O_NONBLOCK
      queueAttr.mq_maxmsg  = queueLength;  // Max. # of messages on queue
      queueAttr.mq_msgsize = itemSize;     // Max. message size (bytes)
      queueAttr.mq_curmsgs = 0;            // # of messages currently in queue

      xQueue = mq_open(queueName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IXUSR, &queueAttr);
   }
   else
   {
      queueAttr.mq_flags   = O_NONBLOCK;   // Flags: 0 or O_NONBLOCK
      queueAttr.mq_maxmsg  = queueLength;  // Max. # of messages on queue
      queueAttr.mq_msgsize = itemSize;     // Max. message size (bytes)
      queueAttr.mq_curmsgs = 0;            // # of messages currently in queue

      xQueue = mq_open(queueName, O_RDWR|O_CREAT|O_NONBLOCK, S_IRUSR|S_IWUSR|S_IXUSR, &queueAttr);
   }

   if( xQueue == -1 )
   {
      printError("CxQueue/%s: queue=%s creation error=%d(%s)!!!", __FUNCTION__, queueName, errno, strerror(errno));
   }
}

CxQueue::~CxQueue()
{
   if (xQueue != -1)
   {
      mq_close(xQueue);
      mq_unlink(queueName);
   }
}

//------------------------------------------------------------------------------

bool CxQueue::send( const void *pItemToQueue, int32_t msg_size )
{
   bool result = false;

   if( xQueue != -1 )
   {  
     result = (0 == mq_send( xQueue, reinterpret_cast<const char*>(pItemToQueue), msg_size, MSG_PRIO));

     if(false == result )
     {
         printError("CxQueue/%s: queue=%s msg_size=%d send error=%d(%s)!!!", __FUNCTION__, queueName, msg_size, errno, strerror(errno));
     }
   }  
   return result;
}

bool CxQueue::timedSend( const void *pItemToQueue, int32_t msg_size, uint64_t time )   
{
   bool result = false;
   const timespec abs_timeout = {(time/1000000000), (time % 1000000000)};

   if( xQueue != -1 )
   {
     result = (0 == mq_timedsend( xQueue, reinterpret_cast<const char*>(pItemToQueue), msg_size, MSG_PRIO, &abs_timeout));
   }
   return result;
}

int32_t CxQueue::receive( void *pItemFromQueue, int32_t msg_size )
{
  int32_t bytes_read = -1;

  if( xQueue != -1 )
  {
      bytes_read = mq_receive(xQueue, reinterpret_cast<char*>(pItemFromQueue), msg_size, NULL);
  }
  return bytes_read;  
}

int32_t CxQueue::occupancy( )
{
   mq_attr tmpAttr; 
   int32_t msg_in_queue = -1;

   if( xQueue != -1 ) 
   {
      if (-1 != mq_getattr(xQueue, &tmpAttr))
      {
         msg_in_queue = tmpAttr.mq_curmsgs;
      }
   }
   return msg_in_queue;
}