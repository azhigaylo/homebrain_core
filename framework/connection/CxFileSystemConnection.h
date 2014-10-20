#ifndef _CX_FILE_SYSTEM_CONNECTION
#define _CX_FILE_SYSTEM_CONNECTION

//------------------------------------------------------------------------------

#include "CxThreadIO.h"
#include "IxFileSystem.h"
#include "CxConnection.h"
#include "..\staticpool\CxVector.h"
#include "..\eventpool\IxEventProducer.h"

//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TFileConnectionItem
{
  int ID;  
  FIL fileData;
};
#pragma pack ( )


typedef TFileConnectionItem *pTFileConnectionItem;

//------------------------------------------------------------------------------

class CxFileSystemConnection : public CxConnection, public CxThreadIO, public IxEventProducer
{
 public:

    enum TFatDrvCommand
    {
      C_GET_FAT_INTERFACE = 1,
      C_EVENT_SD_NOT_READY,
      C_EVENT_SD_READY
    };
   
   enum TState
   {
      ST_U_UNKNOWN = 0,
      ST_U_REQ_INTERFACE,
      ST_U_WAIT_INTERFACE,
      ST_U_NORMAL_OPERATION
   };   
      
   CxFileSystemConnection( char * connectionName, char * driverName );
   ~CxFileSystemConnection();

   void StartConnection();
   
   virtual int open     ( const char*, unsigned char );		                // Open or create a file 
   virtual int close    ( int );					        // Close file    
   virtual int read     ( int, void*, unsigned short );	                        // Read file 
   virtual int write    ( int, const void*, unsigned short );                   // Write file 
   virtual int lseek    ( int, unsigned long );				        // Seek file pointer  
   virtual int opendir  ( DIR*, const char* );                                  // Open a directory    
   virtual int readdir  ( DIR*, FILINFO* );				        // Read a directory item       
   virtual int stat     ( const char*, FILINFO* );				// Get file status          
   virtual int getfree  ( unsigned long* );					// Get number of free clusters  
   virtual int unlink   ( const char* );					// Delete a file or directory 
   virtual int mkdir    ( const char* );	    			        // Create a directory 
   virtual int chmod    ( const char*, unsigned char, unsigned char );	        // Change file attriburte    
   virtual int rename   ( const char*, const char* );		                // Rename a file or directory 
      
 protected: 
   
   virtual void ThreadProcessor();
   virtual void CommandProcessor( TCommand &Command );
   
 private:    
   
   bool IsThisFileInFileList( const char* fileName, pTFileConnectionItem pFileItem );
   bool IsThisFileInFileList( int fileID, pTFileConnectionItem pFileItem );
   bool ChangeFileDataInFileList( pTFileConnectionItem pFileItem );
   
   bool RemFileItemFromList ( int fileID );
   
   // state of the system
   TState State;                                                                   
   // fat interface    
   pIxFileSystem pFileSystem;
   // active file list
   CxVector<TFileConnectionItem> FILE_CONNECTION_LIST; 
   
   bool bRemoteFatState;                                                        // true - mounted
    
}; typedef CxFileSystemConnection *pCxFileSystemConnection;

#endif /*_CX_FILE_SYSTEM_CONNECTION*/   
