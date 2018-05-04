//------------------------------------------------------------------------------
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxQueue.h"
//------------------------------------------------------------------------------
#define MSG_PRIO 1
//------------------------------------------------------------------------------

CxQueue::CxQueue( const char *name, int32_t queueLength, int32_t itemSize, bool blocked ):
    xQueue( -1 )
{
   queueName[0] = '/';
   strncpy( &queueName[1], const_cast<char*>(name), configMAX_QUEUE_NAME_LEN-1 );

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
      if (-1 != mq_close(xQueue))
      {
         if (-1 != mq_unlink(queueName))
         {
            printDebug("CxQueue/%s: queue=%s removed", __FUNCTION__, queueName);
         }
         else
         {
            printWarning("CxQueue/%s: Unable mq_close for queue=%s: %s", __FUNCTION__, queueName, strerror(errno));
         }
      }
      else
      {
         printWarning("CxQueue/%s: Unable mq_close for queue=%s: %s", __FUNCTION__, queueName, strerror(errno));
      }
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

bool CxQueue::timedSend( const void *pItemToQueue, int32_t msg_size, int32_t time )
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
      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(xQueue, &fds);

      struct timeval tv;
      tv.tv_sec = 1;
      tv.tv_usec = 0;

      const int ret = select(xQueue+1, &fds, NULL, NULL, &tv);
      //Check if our file descriptor has received data.
      if (ret > 0 && FD_ISSET(xQueue, &fds))
      {
        bytes_read = static_cast<int32_t>(mq_receive(xQueue, reinterpret_cast<char*>(pItemFromQueue), msg_size, NULL));
      }
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
         msg_in_queue = static_cast<int32_t>(tmpAttr.mq_curmsgs);
      }
   }
   return msg_in_queue;
}
