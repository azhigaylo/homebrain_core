#include "PSysI.h"

//------------------- portable system interface -------------------------------

int FileCreate( char *FileName, char mode )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.CreateFile(FileName, mode) );
}

int FileOpen( char *FileName, char mode )
{   
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.OpenFile(FileName, mode) );
}

int FileClose( int fileID )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.CloseFile( fileID ) );
}

int FileWrite( int fileID, char *buff, unsigned short size )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.WriteFile( fileID, buff, size ) );
}

int FileRead( int fileID, char *buff, unsigned short size )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.ReadFile( fileID, buff, size ) );
}

int FileSeek( int fileID, unsigned long pos )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.SeekFile( fileID, pos ) );
}

void GetCommState( int fileID, pDCB pointerOnDCB )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   ConnectionManager.GetCommState( fileID, pointerOnDCB );
}

bool SetCommState( int fileID, pDCB pointerOnDCB )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   return( ConnectionManager.SetCommState( fileID, pointerOnDCB ) );   
   
}

void GetCommEvent( int fileID, pECB pointerOnECB )
{
   CxConnectionManager &ConnectionManager = CxConnectionManager::getInstance();
   ConnectionManager.GetCommEvent( fileID, pointerOnECB );
}
