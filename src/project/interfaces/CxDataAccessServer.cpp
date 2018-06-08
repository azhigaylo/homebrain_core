//------------------------------------------------------------------------------
// was used code from this repo
// https://github.com/davidsteinsland/cpp-sockets/tree/master/net
//------------------------------------------------------------------------------

#include "interfaces/CxDataAccessServer.h"

#include <poll.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common/slog.h"
#include "common/utils.h"
//------------------------------------------------------------------------------

#define DEFAULT_SOCKET_BUFFER 1024

//------------------------------------------------------------------------------

CxDataClient::CxDataClient(int socket_fd, struct sockaddr_in addr)
   : IxRunnable ("socket_connection")
   , IxEventConsumer( )
   , socketfd   (socket_fd)
   , address    (addr)
{
   sockaddr = new socketaddress(addr);

   setNotification( EVENT_DP_NEW_VALUE );
   setNotification( EVENT_AP_NEW_VALUE );

   // start task processing
   task_run();

   printDebug("CxDataClient/%s: created...", __FUNCTION__);
}

CxDataClient::~CxDataClient()
{
   clrNotification( EVENT_DP_NEW_VALUE );
   clrNotification( EVENT_AP_NEW_VALUE );

   delete sockaddr;
   close();
   printDebug("CxDataClient/%s: removed...", __FUNCTION__);
}

void CxDataClient::set_blocking()
{
   int opts = fcntl(socketfd, F_GETFL);
   opts = opts & (~O_NONBLOCK);
   fcntl(socketfd, F_SETFL, opts);
}

void CxDataClient::set_unblocking()
{
   fcntl(socketfd, F_SETFL, O_NONBLOCK);
}

void CxDataClient::close()
{
   if (-1 != socketfd)
   {
       ::close(socketfd);
       task_stop();
       socketfd = -1;
   }
}

int CxDataClient::read(char* buf, int len)
{
   return ::recv(socketfd, buf, len, 0);
}

int CxDataClient::send(const char* buf, int len, int flags)
{
   return ::send(socketfd, buf, len, flags);
}

int CxDataClient::recv_timeout(int s, int timeout)
{
    int size_recv , total_size= 0;
    struct timeval begin, now;
    const int CHUNK_SIZE = 512;
    char chunk[CHUNK_SIZE];
    double timediff;

    //make socket non blocking
    fcntl(s, F_SETFL, O_NONBLOCK);

    //beginning time
    gettimeofday(&begin , 0);

    while(1)
    {
        gettimeofday(&now , 0);

        //time elapsed in seconds
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

        //if you got some data, then break after timeout
        if( total_size > 0 && timediff > timeout )
        {
            break;
        }

        //if you got no data at all, wait a little longer, twice the timeout
        else if( timediff > timeout*2)
        {
            break;
        }

        memset(chunk ,0 , CHUNK_SIZE);  //clear the variable
        if((size_recv =  ::recv(s , chunk , CHUNK_SIZE , 0) ) < 0)
        {
            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
            usleep(100000);
        }
        else
        {
            total_size += size_recv;
            printf("%s" , chunk);
            //reset beginning time
            gettimeofday(&begin , NULL);
        }
    }

    return total_size;
}


void CxDataClient::process_client_rq(const TClientRequest& rq)
{
   switch(rq.cmd)
   {
      case GetDiscretPoint:
      {
         process_get_d_point(rq.start_point, rq.number_point);
         break;
      }
      case GetAnalogPoint:
      {
         process_get_a_point(rq.start_point, rq.number_point);
         break;
      }
      case SetDiscretPoint:
      {
         process_set_d_point(rq.start_point, rq.point.digital);
         break;
      }
      case SetAnalogPoint:
      {
         process_set_a_point(rq.start_point, rq.point.analog);
         break;
      }
      default : break;
   }
}

void CxDataClient::process_get_d_point(uint16_t /*start_point*/, uint16_t /*number_point*/)
{

}

void CxDataClient::process_get_a_point(uint16_t /*start_point*/, uint16_t /*number_point*/)
{

}

void CxDataClient::process_set_d_point(uint16_t /*start_point*/, const TDPOINT& /*dp*/)
{

}

void CxDataClient::process_set_a_point(uint16_t /*start_point*/, const TAPOINT& /*ap*/)
{

}

void CxDataClient::TaskProcessor()
{
    // use the poll system call to be notified about socket status changes
    struct pollfd pfd;
    pfd.fd = socketfd;
    pfd.events = POLLIN | POLLHUP | POLLRDNORM;
    pfd.revents = 0;

    // call poll with a timeout of 100 ms
    if (poll(&pfd, 1, 100) > 0)
    {
        // if result > 0, this means that there is either data available on the
        // socket, or the socket has been closed
        TClientRequest client_request;
        ssize_t data_size = ::recv(socketfd, &client_request, sizeof(client_request), MSG_PEEK | MSG_DONTWAIT);
        if (0 < data_size )
        {
           // read operation ok, check for full package size
           if (static_cast<ssize_t>(sizeof(client_request)) <= data_size )
           {
              printDebug("CxDataServer/%s: ------got request data------", __FUNCTION__);
              data_size = ::recv(socketfd, &client_request, sizeof(client_request), 0);
           }
        }
        else
        {
            // if recv returns zero, that means the connection has been closed
            close();
        }
    }
}

bool CxDataClient::processEvent( pTEvent /*pEvent*/ )
{

   return true;
}

//------------------------------------------------------------------------------

CxDataServer::CxDataServer(  const char *interfaceName, int port, std::string address)
   : CxInterface    ( interfaceName )
   , IxRunnable     ( interfaceName )
   , m_port    (port)
   , m_backlog (10)
   , m_address (address)
{

}

CxDataServer::~CxDataServer()
{
   close();
}

int32_t CxDataServer::open( )
{
   // start task processing
   task_run();
   // start porp listening
   listen();
   return 0;
}

int32_t CxDataServer::close( )
{
   task_stop();
   server_close();
   closeConnections();

   return 0;
}

int CxDataServer::listen()
{
   socketaddress* sockaddr = new socketaddress(m_address, m_port);
   struct sockaddr_in addr = sockaddr->get_struct();

   m_socketfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (m_socketfd == -1) return errno;

   int yes = 1;
   if (::setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0)
   {
      server_close();
      return errno;
   }

   if (::bind(m_socketfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) != 0)
   {
      server_close();
      return errno;
   }

   if (::listen(m_socketfd, m_backlog) != 0)
   {
      server_close();
      return errno;
   }

   printDebug("CxDataServer/%s: start listening...", __FUNCTION__);

   return 0;
}

CxDataClient* CxDataServer::accept()
{
   struct sockaddr_in from;
   socklen_t l = sizeof(from);
   CxDataClient* pConnection = nullptr;

   printDebug("CxDataServer/%s: wait for connection...", __FUNCTION__);

   fd_set fds;
   FD_ZERO(&fds);
   FD_SET(m_socketfd, &fds);

   struct timeval timeout;
   timeout.tv_sec  = 1;
   timeout.tv_usec = 0;

   const int ret = select(m_socketfd+1, &fds, NULL, NULL, &timeout);

   //Check if our file descriptor has received data.
   if (ret > 0 && FD_ISSET(m_socketfd, &fds))
   {
       int clientfd = ::accept(m_socketfd, (struct sockaddr*)&from, &l);

       printDebug("CxDataServer/%s: client accepted...", __FUNCTION__);

       pConnection = new CxDataClient(clientfd, from);
   }

   return pConnection;
}

void CxDataServer::TaskProcessor()
{
   CxDataClient* client = accept();

   if (nullptr != client)
   {
       if (!client->valid())
       {
          printDebug("CxDataServer/%s: -------client invalid, try to remove them----", __FUNCTION__);
          delete client;
       }
       else
       {
           m_connection_list.push_back( client );
       }
   }

   checkConnections();

   sleep_mcs(500);
}

void CxDataServer::checkConnections()
{
   for( std::vector<CxDataClient*>::iterator it = m_connection_list.begin(); it != m_connection_list.end(); it++ )
   {
      if (false == (*it)->valid())
      {
         printDebug("CxDataServer/%s: -------client invalid, try to remove them----", __FUNCTION__);
         delete *it;
         m_connection_list.erase(it);
         break;
      }
   }
}

void CxDataServer::closeConnections()
{
   for( std::vector<CxDataClient*>::iterator it = m_connection_list.begin(); it != m_connection_list.end(); it++  )
   {
      delete *it;
   }

   m_connection_list.clear();
}

