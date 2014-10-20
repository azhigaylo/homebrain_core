#ifndef _CX_CONNECTIONMANAGER
#define _CX_CONNECTIONMANAGER

//------------------------------------------------------------------------------

#include "CxSerialConnection.h"
#include "..\staticpool\CxList.h"
#include "..\eventpool\CxEvent.h"

//------------------------------------------------------------------------------

#define maxConnectionName 21

//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TConnectionItem
{
  char number;
  char Name[maxConnectionName];
  int  ID;
  pIxConnection pConnection;
};
#pragma pack ( )

typedef TConnectionItem *pConnectionItem;

//------------------------------------------------------------------------------

class CxConnectionManager
{
 public: 

   static CxConnectionManager & getInstance();
  
   virtual ~CxConnectionManager(){}
   
   int CreateFile ( char *FileName, char mode );                                // w - write / r - read
   int OpenFile   ( char *FileName, char mode );                                // w - write / r - read
   int CloseFile  ( int fileID );
   int WriteFile  ( int fileID, char *buff, unsigned short size );
   int ReadFile   ( int fileID, char *buff, unsigned short size );   
   
   // only for file processing use
   int SeekFile   ( int fileID, unsigned long postion );                        // Seek file  
   int StatusFile ( const char*, FILINFO* );				        // Get file status          
   int DeleteFile ( const char* );					        // Delete a file or directory 
   int ChmodFile  ( const char*, unsigned char, unsigned char );	        // Change file attriburte    
   int RenameFile ( const char*, const char* );		                        // Rename a file or directory       
   int OpenDir    ( DIR*, const char* );                                        // Open a directory 
   int ReadDir    ( DIR*, FILINFO* );				                // Read a directory item    
   int MkDir      ( const char* );	    			                // Create a directory 


   
   void GetCommState( int fileID, pDCB pointerOnDCB ); 
   bool SetCommState( int fileID, pDCB pointerOnDCB ); 
   void GetCommEvent( int fileID, pECB pointerOnECB );
   
   bool registration_connection ( pIxConnection pNewConnection );
   void activateAllConnection();
   
 private:
  
    CxConnectionManager( );
       
    CxList<TConnectionItem> CONNECTION_LIST;

    char serialConnectionCounter;
    char ffsConnectionCounter;
    
}; typedef CxConnectionManager * pCxConnectionManager;


#endif /*_CX_CONNECTIONMANAGER*/   
