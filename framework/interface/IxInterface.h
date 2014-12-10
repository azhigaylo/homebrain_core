#ifndef _IX_CONNECTION
#define _IX_CONNECTION

//------------------------------------------------------------------------------

#include "IxFileSystem.h"

//------------------------------------------------------------------------------

class IxConnection
{
 public: 

   IxConnection();
   ~IxConnection(){}
           
   virtual int open     ( const char*, unsigned char ) = 0;		        // Open or create a file 
   virtual int read     ( int, void*, unsigned short ) = 0;	                // Read file 
   virtual int write    ( int, const void*, unsigned short ) = 0;	        // Write file 
   virtual int lseek    ( int, unsigned long ) = 0;				// Seek file pointer   
   virtual int close    ( int ) = 0;						// Close file    
   virtual int opendir  ( DIR*, const char* ) = 0;				// Open a directory 
   virtual int readdir  ( DIR*, FILINFO* ) = 0;					// Read a directory item    
   virtual int stat     ( const char*, FILINFO* ) = 0;				// Get file status          
   virtual int getfree  ( unsigned long* ) = 0;					// Get number of free clusters
   virtual int unlink   ( const char* ) = 0;					// Delete a file or directory 
   virtual int mkdir    ( const char* ) = 0;					// Create a directory    
   virtual int chmod    ( const char*, unsigned char, unsigned char ) = 0;	// Change file attriburte 
   virtual int rename   ( const char*, const char* ) = 0;		        // Rename a file or directory 
   
   virtual void StartConnection() = 0;
   
   virtual int activate( )   = 0;
   virtual int deactivate( ) = 0;   

}; typedef IxConnection *pIxConnection;

#endif /*_IX_CONNECTION*/   
