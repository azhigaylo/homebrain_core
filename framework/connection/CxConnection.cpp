#include "CxConnection.h"
#include "CxConnectionManager.h"

//------------------------------------------------------------------------------

CxConnection::CxConnection( TConnectionType ConnectionType ):
  ConnectionType ( ConnectionType ),
  ConnectionState( ScNotActiv )
{  
   ConnectConnection( );     
}

bool CxConnection::ConnectConnection()
{
  CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance() ;
  
  bool result = ConnectionManager.registration_connection( reinterpret_cast<pIxConnection>(this) );
   
  return result;
}

int CxConnection::activate() 
{
  int result = -1;
  if( ConnectionState == ScNotActiv )
  {
    SetConnectionState( ScClose );
    result = 1; 
  }    
  return result;
}

int CxConnection::deactivate() 
{
  int result = -1;
  if( ConnectionState != ScNotActiv )
  {
    SetConnectionState( ScNotActiv );
    result = 1;
  }    
  return result;
}

int CxConnection::open( const char*, unsigned char )
{
  int result = -1;
  if( ConnectionState == ScClose )
  {
    SetConnectionState( ScOpen );
    result = 1;
  }    
  return result;
}

int CxConnection::close( int fileID )
{
  int result = -1;
  if( ConnectionState == ScOpen )
  {
    SetConnectionState( ScClose );
    result = 1;
  }    
  if( ConnectionState == ScNotActiv )
  {
    result = 1;
  }     
  return result;
}

int CxConnection::read( int fileID, void*, unsigned short )
{
  return -1;
}

int CxConnection::write( int fileID, const void*, unsigned short )
{
  return -1;
}

int CxConnection::lseek( int fileID, unsigned long )
{
  return -1;
}

int CxConnection::opendir( DIR*, const char* )
{
  return -1;
}

int CxConnection::readdir( DIR*, FILINFO* )
{
  return -1;
}

int CxConnection::stat( const char*, FILINFO* )
{
  return -1;
}

int CxConnection::getfree( unsigned long* )
{
  return -1;
}

int CxConnection::unlink( const char* )
{
  return -1;
}

int CxConnection::mkdir( const char* )
{
  return -1;
}

int CxConnection::chmod( const char*, unsigned char, unsigned char )
{
  return -1;
}

int CxConnection::rename( const char*, const char* )
{
  return -1;
}
