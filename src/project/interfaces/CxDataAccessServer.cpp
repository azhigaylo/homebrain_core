//------------------------------------------------------------------------------
// was used code from this repo
// https://github.com/davidsteinsland/cpp-sockets/tree/master/net
//------------------------------------------------------------------------------

#include "interfaces/CxDataAccessServer.h"

#include <poll.h>
#include <fcntl.h>

#include "common/slog.h"
#include "common/utils.h"
#include "interfaces/DataDB.h"
//------------------------------------------------------------------------------

#define DEFAULT_SOCKET_BUFFER 1024

//------------------------------------------------------------------------------

CxDataSocket::CxDataSocket(int socket_fd, struct sockaddr_in addr)
   : IxRunnable ("socket_connection")
   , socketfd   (socket_fd)
   , address    (addr)
{
   sockaddr = new socketaddress(addr);

   // start task processing
   task_run();

   printDebug("CxDataSocket/%s: created...", __FUNCTION__);
}

CxDataSocket::~CxDataSocket()
{
   delete sockaddr;
   close();
   printDebug("CxDataSocket/%s: removed...", __FUNCTION__);
}

void CxDataSocket::set_blocking()
{
   int opts = fcntl(socketfd, F_GETFL);
   opts = opts & (~O_NONBLOCK);
   fcntl(socketfd, F_SETFL, opts);
}

void CxDataSocket::set_unblocking()
{
   fcntl(socketfd, F_SETFL, O_NONBLOCK);
}

void CxDataSocket::close()
{
   if (-1 != socketfd)
   {
       ::close(socketfd);
       task_stop();
       socketfd = -1;
   }
}

int CxDataSocket::read(char* buf, int len)
{
   return ::recv(socketfd, buf, len, 0);
}

int CxDataSocket::send(const char* buf, int len, int flags)
{
   return ::send(socketfd, buf, len, flags);
}

void CxDataSocket::TaskProcessor()
{
    // use the poll system call to be notified about socket status changes
    struct pollfd pfd;
    pfd.fd = socketfd;
    pfd.events = POLLIN | POLLHUP | POLLRDNORM;
    pfd.revents = 0;

    // call poll with a timeout of 100 ms
    if (poll(&pfd, 1, 300) > 0)
    {
        // if result > 0, this means that there is either data available on the
        // socket, or the socket has been closed
        char buffer[32];
        if (0 != ::recv(socketfd, buffer, sizeof(buffer), 0)) // MSG_PEEK | MSG_DONTWAIT
        {
           printDebug("CxDataServer/%s: ------got new data------", __FUNCTION__);
        }
        else
        {
            // if recv returns zero, that means the connection has been closed
            close();
        }
    }
}
//------------------------------------------------------------------------------


CxDataServer::CxDataServer(  const char *interfaceName, int port, std::string address)
   : CxInterface    ( interfaceName )
   , IxRunnable     ( interfaceName )
   , IxEventConsumer( )
   , m_port    (port)
   , m_backlog (10)
   , m_address (address)
{
   setNotification( EVENT_DP_NEW_VALUE );
   setNotification( EVENT_AP_NEW_VALUE );
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

CxDataSocket* CxDataServer::accept()
{
   struct sockaddr_in from;
   socklen_t l = sizeof(from);
   CxDataSocket* pConnection = nullptr;

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

       pConnection = new CxDataSocket(clientfd, from);
   }

   return pConnection;
}

void CxDataServer::TaskProcessor()
{
   CxDataSocket* client = accept();

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
   for( std::vector<CxDataSocket*>::iterator it = m_connection_list.begin(); it != m_connection_list.end(); it++ )
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
   for( std::vector<CxDataSocket*>::iterator it = m_connection_list.begin(); it != m_connection_list.end(); it++  )
   {
      delete *it;
   }

   m_connection_list.clear();
}

bool CxDataServer::processEvent( pTEvent /*pEvent*/ )
{

   return true;
}
