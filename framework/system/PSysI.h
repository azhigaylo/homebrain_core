#ifndef _PSYSI
#define _PSYSI

#include "..\connection\CxConnectionManager.h"

int FileCreate( char *FileName, char mode );
int FileOpen ( char *FileName, char mode);
int FileClose( int fileID );
int FileWrite( int fileID, char *buff, unsigned short size );
int FileRead ( int fileID, char *buff, unsigned short size );
int FileSeek( int fileID, unsigned long pos );
void GetCommState( int fileID, pDCB pointerOnDCB );
bool SetCommState( int fileID, pDCB pointerOnDCB );
void GetCommEvent( int fileID, pECB pointerOnECB );

#endif /*_PSYSI */
