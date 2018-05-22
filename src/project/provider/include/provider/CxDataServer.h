#ifndef _CX_DATA_SERVER
#define _CX_DATA_SERVER

//------------------------------------------------------------------------------

#include <string>
#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

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

class CxDataSocket
{
   public:
      CxDataSocket() {}

      /**
       * Creates a socket instance based upon an already existing
       * socket file descriptor and sockaddr_in structure.
       * Used for example after a call to ::accept()
       * @param the socket file descriptor
       * @param the address structure
       */
      CxDataSocket(int socket_fd, struct sockaddr_in addr)
      : socketfd(socket_fd)
      , address (addr)
      {
         sockaddr = new socketaddress(addr);
      }

      ~CxDataSocket();

      /**
       * Reads all data being sent from the client. The function will block until there's
       * data, and then read the rest if any.
       * @param the string object to save the data into
       */
      int read(std::string&);

      /**
       * Reads a specified amount of data into a character pointer
       * @param the character buffer
       * @param the length of the character buffer
       */
      int read(char*, int);

      /**
       * Sends a string to the client
       * @param the string to send
       */
      int send(std::string);

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
      void close() const
      {
        if (socketfd == -1) return;
        ::close(socketfd);
      }

      /**
       * Checks whether the socket is valid
       * @return true if the socket is valid, false otherwise
       */
      bool valid() const
      {
        return socketfd != -1;
      }

      /**
       * Gets the socket file descriptor
       * @return the socket file descriptor
       */
      int get_socket() const
      {
        return socketfd;
      }

      /**
       * Gets the socketaddress instance of the socket, which contains
       * information about the socket's address and port
       * @return the socketaddress instance
       */
      socketaddress* get_socketaddress() const
      {
        return sockaddr;
      }

    protected:

      int socketfd;
      struct sockaddr_in address;
      socketaddress* sockaddr;
};

//------------------------------------------------------------------------------

class CxDataServer
{
    public:
      /**
       * Constructs a listening socket on the specified port.
       * The default address is 0.0.0.0 and the default backlog is 10.
       *
       * @param the port to listen to
       */
      CxDataServer(int port)
      {
         m_port = port;
         m_backlog = 10;
         m_address = "0.0.0.0";
      }

      /**
       * Constructs a listening socket on the specified port with a backlog.
       * The default address is 0.0.0.0.
       *
       * @param the port to listen to
       * @param the number of backlogs
       */
      CxDataServer(int port, int backlog)
      {
         m_port = port;
         m_backlog = backlog;
         m_address = "0.0.0.0";
      }

      /**
       * Constructs a listening socket on the specified port and address
       * @param the port to listen to
       * @param the number of backlogs
       * @param the address to bind to
       */
      CxDataServer(int port, int backlog, std::string address)
      {
        m_port = port;
        m_backlog = backlog;
        m_address = address;
      }

      ~CxDataServer();

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
      void close() const
      {
        if (m_socketfd == -1) return;
        ::close(m_socketfd);
      }

      /**
       * Checks whether or not the socket is valid
       * @return true the socket is valid, false otherwise
       */
      bool valid() const
      {
        return m_socketfd != -1;
      }

      /**
       * Gets the socket file descriptor
       * @return the socket file descriptor
       */
      int get_socket() const
      {
        return m_socketfd;
      }

    protected:

      int m_port;
      int m_backlog;
      std::string m_address;

      int m_socketfd;
};

#endif // _CX_DATA_SERVER
