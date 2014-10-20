//------------------------------------------------------------------------------

#include <stdlib.h>
#include "utils.h"
#include "..\framework\debug\DebugMacros.h"
#include "CxConnectionManager.h"

//------------------------------------------------------------------------------

CxConnectionManager::CxConnectionManager( ):
  CONNECTION_LIST( 5 ),
  serialConnectionCounter( 0 ),
  ffsConnectionCounter( 0 )
{   

}

CxConnectionManager &CxConnectionManager::getInstance( )
{
  static CxConnectionManager theInstance;
  return theInstance;
}

bool CxConnectionManager::registration_connection( pIxConnection pNewConnection )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )

  bool result = false;
  
  TConnectionItem connectionItem;
  
  serialConnectionCounter++;
  connectionItem.number = CONNECTION_LIST.count() + 1;
  //set name
  mod_memset( connectionItem.Name, 0, sizeof(connectionItem.Name), sizeof(connectionItem.Name) );
  
  switch ( (reinterpret_cast<pCxConnection>(pNewConnection))->GetConnectionType() )
  {
    case CxConnection::CtConnectionFFS : 
      {       
        mod_sprintf( connectionItem.Name, sizeof(connectionItem.Name), "SD%b", &serialConnectionCounter ); 
        connectionItem.ID = rand();
        connectionItem.pConnection = pNewConnection;
          
        result = CONNECTION_LIST.add( connectionItem ); 
        break;
      }
    case CxConnection::CtConnectionSerial : 
      { 
        mod_sprintf( connectionItem.Name, sizeof(connectionItem.Name), "COM%b", &serialConnectionCounter );      
        connectionItem.ID = rand();
        connectionItem.pConnection = pNewConnection;
          
        result = CONNECTION_LIST.add( connectionItem );        
        break;        
      }            
    default :
      {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
         break;
      }      
  }   
  
  return result;
}

//------------------------------------------------------------------------------

int CxConnectionManager::CreateFile( char *FileName, char mode )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;
  char exitState = 2;
  TConnectionItem connectionItem;

  // get the state of system
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
     
     if( true == mod_strcmp( FileName, rConnectionItem.Name ) )
     {
       connectionItem = rConnectionItem; 
       exitState = 1;
       break;
     }
     else
     {
       if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
       {
         connectionItem = rConnectionItem;
         exitState = 0;
       }      
     }  
  }   

  // processing result
  switch( exitState )
  {
    case 0 : {      
                result = connectionItem.pConnection->open( FileName, mode );                  
                
                if( result != -1 )
                {
                  DBG_MSG( ("[M] C_M - create OK \n\r", (unsigned short)__LINE__ ));                 
                }  
                else
                {
                 DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }        
                break;
             }
    case 1 : {
                if( 1 == connectionItem.pConnection->open( NULL, 0 ) )
                {
                  result = connectionItem.ID;
                }
                else
                {
                  DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }  
                break;
             }
    case 2 : {
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }
    default :{
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }      
  }  
          
  return result;
}

int CxConnectionManager::OpenFile( char *FileName, char mode )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;
  char exitState = 2;
  TConnectionItem connectionItem;

  // get the state of system
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
     
     if( true == mod_strcmp( FileName, rConnectionItem.Name ) )
     {
       connectionItem = rConnectionItem; 
       exitState = 1;
       break;
     }
     else
     {
       if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
       {
         connectionItem = rConnectionItem;
         exitState = 0;
       }      
     }  
  }   
  
  // processing result
  switch( exitState )
  {
    case 0 : {      
                result = connectionItem.pConnection->open( FileName, mode );
                if( result != -1 )
                {
                  DBG_MSG( ("[M] C_M - open OK \n\r", (unsigned short)__LINE__ ));                 
                }  
                else
                {
                 DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }        
                break;
             }
    case 1 : {
                if( 1 == connectionItem.pConnection->open( NULL, 0 ) )
                {
                  result = connectionItem.ID;
                }
                else
                {
                  DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }  
                break;
             }
    case 2 : {
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }
    default :{
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }      
  }  
          
  return result;
}

int CxConnectionManager::CloseFile( int fileID )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )

  int result = -1;
  char exitState = 2;
  TConnectionItem connectionItem;
  
  // get the state of system
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
     
     if( fileID == rConnectionItem.ID )
     {
       connectionItem = rConnectionItem; 
       exitState = 1;
       break;
     }
     else
     {
       if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
       {
         connectionItem = rConnectionItem;
         exitState = 0;
       }      
     }  
  }   
    
  // processing result
  switch( exitState )
  {
    case 0 : 
    case 1 : {
                result = connectionItem.pConnection->close( fileID );
                if( -1 == result )          
                {
                  DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }       
                break;
             }
    case 2 : {
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }
    default :{
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }      
  }   

  return result;
}

int CxConnectionManager::WriteFile( int fileID, char *buff, unsigned short size )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )

  int result = -1;
  char exitState = 2;
  TConnectionItem connectionItem;

  // get the state of system
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
     
     if( fileID == rConnectionItem.ID )
     {
       connectionItem = rConnectionItem; 
       exitState = 1;
       break;
     }
     else
     {
       if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
       {
         connectionItem = rConnectionItem;
         exitState = 0;
       }      
     }  
  }   
  
  // processing result
  switch( exitState )
  {
    case 0 : {      
                result = connectionItem.pConnection->write( fileID, buff, size  );
                if( result != -1 )
                {
                  DBG_MSG( ("[M] C_M - write OK \n\r", (unsigned short)__LINE__ ));                 
                }  
                else
                {
                 DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }     
                break;
             }
    case 1 : {
                result = connectionItem.pConnection->write( fileID, buff, size );
                if( -1 == result )          
                {
                  DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }  
                break;
             }
    case 2 : {
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }
    default :{
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }      
  }  
 
  return result;
}

int CxConnectionManager::ReadFile( int fileID, char *buff, unsigned short size)
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )

  int result = -1;
  char exitState = 2;
  TConnectionItem connectionItem;

  // get the state of system
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
     
     if( fileID == rConnectionItem.ID )
     {
       connectionItem = rConnectionItem; 
       exitState = 1;
       break;
     }
     else
     {
       if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
       {
         connectionItem = rConnectionItem;
         exitState = 0;
       }      
     }  
  }   
  
  // processing result
  switch( exitState )
  {
    case 0 : {      
                result = connectionItem.pConnection->read( fileID, buff, size  );             
  
                if( result != -1 )
                {
                  DBG_MSG( ("[M] C_M - read OK \n\r", (unsigned short)__LINE__ ));                 
                }  
                else
                {
                 DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                }  
                break;
             }
    case 1 : {
                result = connectionItem.pConnection->read( fileID, buff, size );
                
                if( -1 == result )          
                {
                  DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                } 
                break;
             }
    case 2 : {
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }
    default :{
                DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
                break;
             }      
  }  
  
  return result;
}
  
//------------------------------------------------------------------------------
int CxConnectionManager::SeekFile( int fileID, unsigned long postion )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;
  
  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->lseek( fileID, postion );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - lseek OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }
       break;
     }
  }   
  
  return result;
}
int CxConnectionManager::StatusFile( const char *path, FILINFO *finfo )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->stat( path, finfo );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - stat OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::DeleteFile( const char *path )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->unlink( path );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - delete OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::ChmodFile( const char *path, unsigned char value, unsigned char mask )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->chmod( path, value, mask );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - chmod OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }  
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::RenameFile( const char *path_old, const char *path_new )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->rename( path_old, path_new );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - rename OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }   
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::OpenDir( DIR *scan, const char *path )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->opendir( scan, path );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - opendir OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }   
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::ReadDir( DIR *scan, FILINFO *finfo )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->readdir( scan, finfo );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - readdir OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }    
       break;       
     }
  }   
  return result;
}

int CxConnectionManager::MkDir( const char *path )
{
  DBG_SCOPE( CxConnectionManager, CxConnectionManager )
  
  int result = -1;

  for(char i=0; i < CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
           
     if( CxConnection::CtConnectionFFS == (reinterpret_cast<pCxConnection>(rConnectionItem.pConnection ))->GetConnectionType() ) 
     {
       result = rConnectionItem.pConnection->mkdir( path );             

       if( result != -1 )
       {
         DBG_MSG( ("[M] C_M - mkdir OK \n\r", (unsigned short)__LINE__ ));                 
       }  
       else
       {
         DBG_MSG( ("[E] C_M - %w \n\r", (unsigned short)__LINE__ )); 
       }
       break;
     }
  }   
  return result;
}

   
//------------------------------------------------------------------------------

void CxConnectionManager::GetCommState( int fileID, pDCB pointerOnDCB )
{
  for( char i=0; i<CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
                              
     if( fileID == rConnectionItem.ID )
     {     
        if( rConnectionItem.pConnection != NULL ) 
        { 
          (reinterpret_cast<pCxSerialConnection>(rConnectionItem.pConnection))->getState( pointerOnDCB );
        }
        else
        {

        }         
        break;
     }          
  }  
}

bool CxConnectionManager::SetCommState( int fileID, pDCB pointerOnDCB )
{
  bool result = false;
  
  for( char i=0; i<CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
                              
     if( fileID == rConnectionItem.ID )
     {     
        if( rConnectionItem.pConnection != NULL ) 
        { 
          result = (reinterpret_cast<pCxSerialConnection>(rConnectionItem.pConnection))->setState( pointerOnDCB );
        }
        else
        {

        }         
        break;
     }          
  }  
  return result;
}

void CxConnectionManager::GetCommEvent(  int fileID, pECB pointerOnECB )
{
  for( char i=0; i<CONNECTION_LIST.count(); i++ )
  {
     TConnectionItem &rConnectionItem = CONNECTION_LIST[i];
                              
     if( fileID == rConnectionItem.ID )
     {     
        if( rConnectionItem.pConnection != NULL ) 
        { 
          (reinterpret_cast<pCxSerialConnection>(rConnectionItem.pConnection))->getEvent( pointerOnECB );
        }
        else
        {

        }         
        break;
     }          
  }  
}

//------------------------------------------------------------------------------

void CxConnectionManager::activateAllConnection()
{
  for( char i=0; i<CONNECTION_LIST.count(); i++ )
  {
    CONNECTION_LIST[i].pConnection->StartConnection();       
  }  
}
