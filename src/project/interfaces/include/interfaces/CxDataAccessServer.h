#ifndef _CX_DATA_SERVER
#define _CX_DATA_SERVER

//------------------------------------------------------------------------------

#include <string>
#include <cstring>
#include <cerrno>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "common/ptypes.h"
#include "os_wrapper/IxRunnable.h"
#include "interface/CxInterface.h"
#include "eventpool/IxEventConsumer.h"
//------------------------------------------------------------------------------

class socketaddress
{
   public:
      /**
       * Creates a new socketaddress instance based on a sockaddr_in structure
       * @param the sockaddr_in structure
       */
      socketaddress(struct sockaddr_in addr)
      {
         m_port = addr.sin_port;

         char ip[INET_ADDRSTRLEN];
         inet_ntop(addr.sin_family, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

         m_address = std::string(ip);
      }

      /**
       * Creates a new socketaddress instance with a specified address and port
       * @param the address of the socket
       * @param the port
       */
      socketaddress(std::string address, int port)
      {
         m_address = address;
         m_port = port;
      }

      /**
       * Returns a sockaddr_in structure based on the information of the socketaddress instance
       * @return sockaddr_in structure
       */
      struct sockaddr_in get_struct() const
      {
         struct sockaddr_in addr;
         memset(&addr, 0, sizeof addr);

         addr.sin_family = AF_INET;
         addr.sin_port = htons(m_port);

         inet_aton(m_address.c_str(), &addr.sin_addr);

         return addr;
      }

      /**
       * Gets the port of the socket
       * @return the port number
       */
      int get_port() const
      {
         return m_port;
      }

      /**
       * Gets the address of the socket
       * @return the address
       */
      std::string get_address() const
      {
         return m_address;
      }

   private:

      int m_port;
      std::string m_address;
};

//------------------------------------------------------------------------------

class CxDataSocket : public IxRunnable
{
   public:
      /**
       * Creates a socket instance based upon an already existing
       * socket file descriptor and sockaddr_in structure.
       * Used for example after a call to ::accept()
       * @param the socket file descriptor
       * @param the address structure
       */
      CxDataSocket(int socket_fd, struct sockaddr_in addr);
      ~CxDataSocket();

      /**
       * Reads a specified amount of data into a character pointer
       * @param the character buffer
       * @param the length of the character buffer
       */
      int read(char*, int);

      /**
       * Sends an array of charactes to the client, with a specified start and end index
       * @param the character buffer
       * @param the starting position
       * @param the length
       */
      int send(const char*, int, int);

      /**
       * Sets the socket in blocking mode
       */
      void set_blocking();

      /**
       * Sets the socket in non-blocking mode
       */
      void set_unblocking();

      /**
       * Closes the socket connection
       */
      void close();
      /**
       * Checks whether the socket is valid
       * @return true if the socket is valid, false otherwise
       */
      bool valid() const { return socketfd != -1;}

      /**
       * Gets the socket file descriptor
       * @return the socket file descriptor
       */
      int get_socket() const { return socketfd; }

      /**
       * Gets the socketaddress instance of the socket, which contains
       * information about the socket's address and port
       * @return the socketaddress instance
       */
      socketaddress* get_socketaddress() const { return sockaddr; }

    protected:

      int socketfd;
      struct sockaddr_in address;
      socketaddress* sockaddr;

      virtual void TaskProcessor();

    private:

      CxDataSocket( const CxDataSocket & );
      CxDataSocket & operator=( const CxDataSocket & );
};

//------------------------------------------------------------------------------

class CxDataServer :  public CxInterface, public IxRunnable, public IxEventConsumer
{
    public:

      /**
       * Constructs a listening socket on the specified port and address
       * @param the port to listen to
       * @param the address to bind to
       */
      CxDataServer( const char *interfaceName, int port, std::string address);

      virtual ~CxDataServer();

      virtual int32_t open  ( );
      virtual int32_t close ( );

      /**
       * Creates the listening socket and binds to the current port and address
       * @return error code if there was a problem
       */
      int listen();

      /**
       * Accepts a new incomming client (blocking call).
       * @return A client socket instance
       */
      CxDataSocket* accept();

      /**
       * Closes the listening socket
       */
      void server_close() const { if (-1 != m_socketfd) ::close(m_socketfd); }

      /**
       * Checks whether or not the socket is valid
       * @return true the socket is valid, false otherwise
       */
      bool valid() const { return m_socketfd != -1; }

      /**
       * Gets the socket file descriptor
       * @return the socket file descriptor
       */
      int get_socket() const { return m_socketfd; }

    protected:

      int m_port;
      int m_backlog;
      std::string m_address;

      int m_socketfd;

      virtual void TaskProcessor();
      virtual bool processEvent( pTEvent pEvent );

    private:
      CxDataServer( const CxDataServer & );
      CxDataServer & operator=( const CxDataServer & );

      void checkConnections();
      void closeConnections();

      std::vector<CxDataSocket*> m_connection_list;
};

#endif // _CX_DATA_SERVER
