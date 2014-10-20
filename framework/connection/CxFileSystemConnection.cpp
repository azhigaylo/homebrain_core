#include "CxFileSystemConnection.h"
#include "..\staticpool\CxStaticPool.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxFileSystemConnection::CxFileSystemConnection( char * connectionName, char * driverName ):
   CxConnection ( CxConnection::CtConnectionFFS )
  ,CxThreadIO   ( connectionName, driverName )  
  ,State        ( ST_U_UNKNOWN ) 
  ,pFileSystem  ( NULL )  
  ,FILE_CONNECTION_LIST ( 5 )
  ,bRemoteFatState( false )  

{  
  SetConnectionState( CxConnection::ScOpen );
}

CxFileSystemConnection::~CxFileSystemConnection( )
{

}

//----------------user function-------------------------------------------------

int CxFileSystemConnection::open( const char* name, unsigned char mode )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
  
  int result = -1;
  unsigned short fileID = CRC16_T( name, mod_strlen(name, 50) );
    
  if( (pFileSystem != NULL) && (name != NULL) && (true == bRemoteFatState) )
  {  
      // create new item in the file list
      TFileConnectionItem fileConnectionItem;      
      
      if( false == IsThisFileInFileList( name, &fileConnectionItem ) )
      {          
        unsigned short openResult = pFileSystem->f_open( &fileConnectionItem.fileData, name, mode );                                
        if( FR_OK == openResult ) 
        {              
          fileConnectionItem.ID = fileID;
          bool retResult = FILE_CONNECTION_LIST.add( fileConnectionItem );
          
          if( true == retResult )
          {
            result = fileID;
          }                    
        }          
        DBG_MSG( ("[M] file open = %w \n\r", openResult) ); 
      }
      else
      {
        DBG_MSG( ("[M] file already open \n\r") ); 
      }                  
  }   
 
  return result;
}

int CxFileSystemConnection::close( int fileID )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
  
  int result = -1;

  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
      // create new item in the file list
      TFileConnectionItem fileConnectionItem;
          
      if( true == IsThisFileInFileList( fileID, &fileConnectionItem ) )
      {
        unsigned short openResult = pFileSystem->f_close( &fileConnectionItem.fileData );                                
        if( FR_OK == openResult ) 
        {              
          bool retResult = RemFileItemFromList( fileID );
          
          if( true == retResult )
          {
            result = 0;
          }                    
        }          
        DBG_MSG( ("[M] file closed = %w \n\r", openResult) ); 
      }
      else
      {
        DBG_MSG( ("[M] file not in list \n\r") ); 
      }                  
  }   
 
  return result;
}
   
int CxFileSystemConnection::read( int fileID, void* buffer, unsigned short size )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;
  unsigned short realLength = 0;
  
  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
      // create new item in the file list
      TFileConnectionItem fileConnectionItem;
          
      if( true == IsThisFileInFileList( fileID, &fileConnectionItem ) )
      {
        unsigned short openResult = pFileSystem->f_read( &fileConnectionItem.fileData, buffer, size, &realLength  );                                
        if( FR_OK == openResult ) 
        {              
           result = realLength; 
           ChangeFileDataInFileList( &fileConnectionItem );
        }          
        DBG_MSG( ("[M] file read = %w \n\r", openResult) ); 
      }
      else
      {
        DBG_MSG( ("[M] file is not opened \n\r") ); 
      }                  
  }   
  
  return result;
}

int CxFileSystemConnection::write( int fileID, const void* buffer, unsigned short size )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;
  unsigned short realLength = 0;
  
  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
      // create new item in the file list
      TFileConnectionItem fileConnectionItem;
          
      if( true == IsThisFileInFileList( fileID, &fileConnectionItem ) )
      {
        unsigned short openResult = pFileSystem->f_write( &fileConnectionItem.fileData, buffer, size, &realLength  );                                
        if( FR_OK == openResult ) 
        {              
           result = realLength; 
           ChangeFileDataInFileList( &fileConnectionItem );
        }          
        DBG_MSG( ("[M] file write = %w \n\r", openResult) ); 
      }
      else
      {
        DBG_MSG( ("[M] file is not opened \n\r") ); 
      }                  
  }   
  
  return result;
}

//------------------------------------------------------------------------------

int CxFileSystemConnection::lseek( int fileID, unsigned long pos )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
      // create new item in the file list
      TFileConnectionItem fileConnectionItem;
          
      if( true == IsThisFileInFileList( fileID, &fileConnectionItem ) )
      {
        unsigned short seekResult = pFileSystem->f_lseek( &fileConnectionItem.fileData, pos );                                
        if( FR_OK == seekResult ) 
        {              
           result = seekResult; 
           ChangeFileDataInFileList( &fileConnectionItem );
        }          
        DBG_MSG( ("[M] file write = %w \n\r", seekResult) ); 
      }
      else
      {
        DBG_MSG( ("[W] file is not opened \n\r") ); 
      }                  
  }   
  
  return result;
}

int CxFileSystemConnection::opendir( DIR *scan, const char *path )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( (pFileSystem != NULL) && (true == bRemoteFatState) )
  {   
    unsigned short openkResult = pFileSystem->f_opendir( scan, path );                                
    if( FR_OK != openkResult ) 
    {              
      DBG_MSG( ("[W] opendir error \n\r") );       
    }
    else
    {
      result = 0;
    }  
  }
  return result;
}

int CxFileSystemConnection::readdir( DIR *scan, FILINFO*fileinfo )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( (pFileSystem != NULL) && (true == bRemoteFatState) )
  {   
    unsigned short readdirResult = pFileSystem->f_readdir( scan, fileinfo );                                
    if( FR_OK != readdirResult ) 
    {              
      DBG_MSG( ("[W] readdir error \n\r") );       
    }    
    else
    {
      result = 0;
    }     
  }
  return result;
}

int CxFileSystemConnection::stat( const char* path, FILINFO*fileinfo )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( (pFileSystem != NULL) && (true == bRemoteFatState) )
  {   
    unsigned short statResult = pFileSystem->f_stat( path, fileinfo );                                
    if( FR_OK != statResult ) 
    {              
      DBG_MSG( ("[W] stat error \n\r") );       
    }     
    else
    {
      result = 0;
    }     
  }
  return result;
}

int CxFileSystemConnection::getfree( unsigned long*length )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( (pFileSystem != NULL) && (true == bRemoteFatState) )
  {   
    unsigned short freeResult = pFileSystem->f_getfree( length );                                
    if( FR_OK != freeResult ) 
    {              
      DBG_MSG( ("[W] getfree error \n\r") );       
    }     
    else
    {
      result = 0;
    }     
  }
  return result;
}

int CxFileSystemConnection::unlink( const char*path )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
     // create new item in the file list
     TFileConnectionItem fileConnectionItem;
        
     if( true != IsThisFileInFileList( path, &fileConnectionItem ) )
     {
       unsigned short unlinkResult = pFileSystem->f_unlink( path );                                
       if( FR_OK == unlinkResult ) 
       {              
          result = 0;                
       }          
       DBG_MSG( ("[M] file delete = %w \n\r", unlinkResult) ); 
     }
     else
     {
       DBG_MSG( ("[W] file can't deleted \n\r") ); 
     }                  
  }   
  
  return result;
}

int CxFileSystemConnection::mkdir( const char*dir )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( (pFileSystem != NULL) && (true == bRemoteFatState) )
  {   
    unsigned short mkdirResult = pFileSystem->f_mkdir( dir );                                
    if( FR_OK != mkdirResult ) 
    {              
      DBG_MSG( ("[W] mkdir error \n\r") );       
    }     
    else
    {
      result = 0;
    }     
  }
  return result;
}

int CxFileSystemConnection::chmod( const char *path, unsigned char value, unsigned char mask )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
     // create new item in the file list
     TFileConnectionItem fileConnectionItem;
        
     if( true != IsThisFileInFileList( path, &fileConnectionItem ) )
     {
       unsigned short chmodResult = pFileSystem->f_chmod( path, value, mask );                                
       if( FR_OK == chmodResult ) 
       {              
          result = 0;
          ChangeFileDataInFileList( &fileConnectionItem );
       }          
       DBG_MSG( ("[M] file chmod = %w \n\r", chmodResult) ); 
     }
     else
     {
       DBG_MSG( ("[W] file can't modified \n\r") ); 
     }                  
  }   
  
  return result;
}

int CxFileSystemConnection::rename( const char *path_old, const char *path_new )
{
  DBG_SCOPE( CxFileSystemConnection, CxFileSystemConnection )
    
  int result = -1;

  if( pFileSystem != NULL && (true == bRemoteFatState) )
  {  
     // create new item in the file list
     TFileConnectionItem fileConnectionItem;
        
     if( true != IsThisFileInFileList( path_old, &fileConnectionItem ) )
     {
       unsigned short renameResult = pFileSystem->f_rename( path_old, path_new );                                
       if( FR_OK == renameResult ) 
       {              
          result = 0;   
          ChangeFileDataInFileList( &fileConnectionItem );
       }          
       DBG_MSG( ("[M] file rename = %w \n\r", renameResult) ); 
     }
     else
     {
       DBG_MSG( ("[W] file can't renamed \n\r") ); 
     }                  
  }   
  
  return result;
}


//------------------------------------------------------------------------------

void CxFileSystemConnection::StartConnection()
{    
  DBG_SCOPE( CxSerialConnection, CxSerialConnection )
  
  DBG_MSG( ("[M] connection - %s was started \n\r", pcTaskName) ); 
  
  // create thread
  task_run( );      
 
}

bool CxFileSystemConnection::IsThisFileInFileList( const char* fileName, pTFileConnectionItem pFileItem )
{
  unsigned short fileID = CRC16_T( fileName, mod_strlen(fileName, 50) );
    
  if( (pFileSystem != NULL) && (fileName != NULL) )
  {  
      // get the state of system
      for( char i=0; i < FILE_CONNECTION_LIST.count(); i++ )
      {
         // get item form active file list
         TFileConnectionItem &rFileConnectionItem = FILE_CONNECTION_LIST[i];
         
         if( fileID == rFileConnectionItem.ID )
         {
           *pFileItem = rFileConnectionItem;
            return true; 
         }  
      }               
  }   
  return false;
}

bool CxFileSystemConnection::IsThisFileInFileList( int fileID, pTFileConnectionItem pFileItem )
{
  if( pFileSystem != NULL )
  {  
      // get the state of system
      for( char i=0; i < FILE_CONNECTION_LIST.count(); i++ )
      {
         // get item form active file list
         TFileConnectionItem &rFileConnectionItem = FILE_CONNECTION_LIST[i];
         
         if( fileID == rFileConnectionItem.ID )
         {
            *pFileItem = rFileConnectionItem;
            return true; 
         }  
      }               
  }    
  return false;
}

bool CxFileSystemConnection::ChangeFileDataInFileList( pTFileConnectionItem pFileItem )
{
  if( pFileSystem != NULL )
  {  
      // get the state of system
      for( char i=0; i < FILE_CONNECTION_LIST.count(); i++ )
      {
         // get item form active file list
         TFileConnectionItem &rFileConnectionItem = FILE_CONNECTION_LIST[i];
         
         if( pFileItem->ID == rFileConnectionItem.ID )
         {
            rFileConnectionItem.fileData = pFileItem->fileData;
            return true; 
         }  
      }               
  }    
  return false;
}

bool CxFileSystemConnection::RemFileItemFromList( int fileID )
{
  bool result = false;
  
  if( pFileSystem != NULL )
  {  
      // get the state of system
      for( char i=0; i < FILE_CONNECTION_LIST.count(); i++ )
      {
         // get item form active file list
         TFileConnectionItem &rFileConnectionItem = FILE_CONNECTION_LIST[i];
         
         if( fileID == rFileConnectionItem.ID )
         {
            result = FILE_CONNECTION_LIST.remove( &rFileConnectionItem ); 
         }  
      }               
  }    
  return result;       
}

//------------------------------------------------------------------------------

void CxFileSystemConnection::ThreadProcessor()
{
  switch( State )
  {    
      case ST_U_UNKNOWN : 
      {                 
        State = ST_U_REQ_INTERFACE;                                             // put in next state

        if( pFileSystem != NULL )
        {  
           sendEvent( CxEvent::EVENT_FFS_UNMOUNTED, NULL );
        }          
        break;
      }
      case ST_U_REQ_INTERFACE :  
      {                 
        TCommand Command  = { threadID, drvID, request, C_GET_FAT_INTERFACE, NULL };
        SendCommand( &Command );                 
        State = ST_U_WAIT_INTERFACE;                                            // put in next state
        break;
      }
      case ST_U_WAIT_INTERFACE :  
      {         
        if( NULL != pFileSystem ) 
        {
          State = ST_U_NORMAL_OPERATION;                                        // put in next state

          if( pFileSystem != NULL )
          {  
             sendEvent( CxEvent::EVENT_FFS_MOUNTED, NULL );
          }           
        }  
        break;
      }	    
      case ST_U_NORMAL_OPERATION :  
      {         
        
        break;
      }	      

      default : break;
   }    
}

void CxFileSystemConnection::CommandProcessor( TCommand &Command )
{
  DBG_SCOPE( CxSerialConnection, CxSerialConnection )
    
  switch( Command.ComID )
  {    
      case C_GET_FAT_INTERFACE : 
      {         
        pFileSystem = reinterpret_cast<pIxFileSystem>( Command.Container );
        State = ST_U_WAIT_INTERFACE;                                            // put in next state        
        break;
      }       
      case C_EVENT_SD_NOT_READY : 
      {         
        bRemoteFatState = false;
        
        State = ST_U_UNKNOWN;
        
        DBG_MSG( ("[M] SD unmounted \n\r") ); 
        break;
      }    
      case C_EVENT_SD_READY : 
      {         
        bRemoteFatState = true;
        
        State = ST_U_REQ_INTERFACE;                                             // put in next state
        
        DBG_MSG( ("[M] SD mounted \n\r") ); 
        break;
      }          
      default : break;
  }  
}

