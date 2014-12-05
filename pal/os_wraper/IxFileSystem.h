#ifndef _IX_FILESYSTEM
#define _IX_FILESYSTEM

//------------------------------------------------------------------------------
// File function return code (FRESULT) 

#define	FR_OK                       0
#define	FR_NOT_READY                1
#define	FR_NO_FILE                  2
#define	FR_NO_PATH                  3
#define	FR_INVALID_NAME             4
#define	FR_DENIED                   5
#define	FR_DISK_FULL                6
#define	FR_RW_ERROR                 7
#define	FR_INCORRECT_DISK_CHANGE    8
#define	FR_WRITE_PROTECTED          9
#define	FR_NOT_ENABLED              10
#define	FR_NO_FILESYSTEM            11


// File access control and file status flags (FIL.flag) 

#define	FA_READ                    0x01
#define	FA_OPEN_EXISTING           0x00
#define	FA_WRITE                   0x02
#define	FA_CREATE_ALWAYS           0x08
#define	FA_OPEN_ALWAYS             0x10
#define FA__WRITTEN                0x20
#define FA__DIRTY                  0x40
#define FA__ERROR                  0x80

//------------------------------------------------------------------------------
// Result type for fatfs application interface 
typedef unsigned char   FRESULT;

// File object structure 
typedef struct _FIL 
{
   unsigned long  fptr;         // File R/W pointer 
   unsigned long  fsize;        // File size 
   unsigned long  org_clust;    // File start cluster 
   unsigned long  curr_clust;   // Current cluster 
   unsigned long  curr_sect;    // Current sector 
#ifndef _FS_READONLY
   unsigned long  dir_sect;     // Sector containing the directory entry 
   unsigned char* dir_ptr;      // Ponter to the directory entry in the window 
#endif
   unsigned char  flag;         // File status flags 
   unsigned char  sect_clust;   // Left sectors in cluster 
} FIL;

// Directory object structure 
typedef struct _DIR 
{
   unsigned long  sclust;       // Start cluster
   unsigned long  clust;        // Current cluster 
   unsigned long  sect;         // Current sector 
   unsigned short index;        // Current index 
} DIR;

// File status structure 
typedef struct _FILINFO 
{
   unsigned long fsize;         // Size
   unsigned short fdate;        // Date 
   unsigned short ftime;        // Time 
   unsigned char fattrib;       // Attribute 
   char fname[8+1+3+1];         // Name (8.3 format) 
} FILINFO;

//------------------------------------------------------------------------------

class IxFileSystem
{ 
  public:       

     virtual FRESULT f_open     ( FIL*, const char*, unsigned char ) = 0;                    // Open or create a file 
     virtual FRESULT f_read     ( FIL*, void*, unsigned short, unsigned short* ) = 0;        // Read file 
     virtual FRESULT f_write    ( FIL*, const void*, unsigned short, unsigned short* ) = 0;  // Write file 
     virtual FRESULT f_lseek    ( FIL*, unsigned long ) = 0;                                 // Seek file pointer   
     virtual FRESULT f_close    ( FIL* ) = 0;                                                // Close file    
     virtual FRESULT f_opendir  ( DIR*, const char* ) = 0;                                   // Open a directory 
     virtual FRESULT f_readdir  ( DIR*, FILINFO* ) = 0;                                      // Read a directory item    
     virtual FRESULT f_stat     ( const char*, FILINFO* ) = 0;                               // Get file status          
     virtual FRESULT f_getfree  ( unsigned long* ) = 0;                                      // Get number of free clusters
     virtual FRESULT f_unlink   ( const char* ) = 0;                                         // Delete a file or directory 
     virtual FRESULT f_mkdir    ( const char* ) = 0;                                         // Create a directory    
     virtual FRESULT f_chmod    ( const char*, unsigned char, unsigned char ) = 0;           // Change file attriburte 
     virtual FRESULT f_rename   ( const char*, const char* ) = 0;                            // Rename a file or directory 

  protected:

    // function's   
    IxFileSystem( ) {} 
    ~IxFileSystem( ){}

  private:       

   IxFileSystem( const IxFileSystem & );
   IxFileSystem & operator=( const IxFileSystem & );
   
}; typedef IxFileSystem *pIxFileSystem;

//------------------------------------------------------------------------------

#endif // _IX_FILESYSTEM
