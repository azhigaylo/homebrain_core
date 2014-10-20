#ifndef _CX_CONNECTION
#define _CX_CONNECTION

#include "IxConnection.h"
//------------------------------------------------------------------------------
   
class CxConnection : public IxConnection
{
 
 public: 
     
   enum TConnectionType
   {
       CtConnectionSerial = 1,    
       CtConnectionFFS
   }; 
     
   enum TConnectionState
   {
       ScNotActiv = 1,    
       ScOpen,
       ScClose
   }; 

   CxConnection( TConnectionType ConnectionType );
   ~CxConnection(){}
      
   // for serial & file
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
   virtual int chmod    ( const char*, unsigned char, unsigned char );		// Change file attriburte    
   virtual int rename   ( const char*, const char* );		                // Rename a file or directory 

   virtual void StartConnection(){}
   
   virtual int activate( );
   virtual int deactivate( );
   
   TConnectionState GetConnectionState( ) { return ConnectionState; }      
   TConnectionType  GetConnectionType ( ) { return ConnectionType;  }  

 protected:

   void SetConnectionState ( TConnectionState State ){ ConnectionState = State; }   
      
 private:    

   bool ConnectConnection( );
   
   TConnectionType  ConnectionType;
   TConnectionState ConnectionState;
   
}; typedef CxConnection *pCxConnection;

#endif /*_CX_CONNECTION*/   
