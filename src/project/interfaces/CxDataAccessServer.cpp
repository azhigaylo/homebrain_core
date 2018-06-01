//------------------------------------------------------------------------------
// was used code from this repo
// https://github.com/davidsteinsland/cpp-sockets/tree/master/net
//------------------------------------------------------------------------------

#include "interfaces/CxDataAccessServer.h"

#include <fcntl.h>

#include "common/slog.h"
#include "common/utils.h"

//------------------------------------------------------------------------------

#define DEFAULT_SOCKET_BUFFER 1024

//------------------------------------------------------------------------------

CxDataSocket::~CxDataSocket()
{
   delete sockaddr;
   close();
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

int CxDataSocket::read(std::string& msg)
{
   int bytes_total = 0;
   char buffer[DEFAULT_SOCKET_BUFFER];

   int bytes_read = recv(socketfd, buffer, DEFAULT_SOCKET_BUFFER, 0);

   if (bytes_read <= 0)
   {
      return bytes_read;
   }

   msg.append(std::string(buffer, 0, bytes_read));
   bytes_total += bytes_read;

   // set non-blocking.
   set_unblocking();

   while (bytes_read > 0)
   {
      memset(buffer, 0, DEFAULT_SOCKET_BUFFER);
      bytes_read = recv(socketfd, buffer, DEFAULT_SOCKET_BUFFER, 0);

      if (bytes_read < 0) break;

      msg.append(std::string(buffer, 0, bytes_read));
      bytes_total += bytes_read;
   }

   // set back to blocking
   set_blocking();

   return bytes_total;
}

int CxDataSocket::read(char* buf, int len)
{
   return ::recv(socketfd, buf, len, 0);
}

int CxDataSocket::send(std::string data)
{
   return send(data.c_str(), data.length(), 0);
}

int CxDataSocket::send(const char* buf, int len, int flags)
{
   return ::send(socketfd, buf, len, flags);
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
   server_close();
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

   printDebug("CxDataServer/%s: wait for connection...", __FUNCTION__);

   int clientfd = ::accept(m_socketfd, (struct sockaddr*)&from, &l);

   printDebug("CxDataServer/%s: client accepted...", __FUNCTION__);

   return new CxDataSocket(clientfd, from);
}

void CxDataServer::TaskProcessor()
{
   CxDataSocket* client = accept();

   if (!client->valid())
   {
        delete client;
    }
   sleep_mcs(500);
}

bool CxDataServer::processEvent( pTEvent /*pEvent*/ )
{

   return true;
}
