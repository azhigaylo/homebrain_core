#ifndef _CX_SD_FAT_FS
#define _CX_SD_FAT_FS

//------------------------------------------------------------------------------

#include "Utils.h"
#include "CxMutex.h"
#include "IxFileSystem.h"
#include "..\bus manager\CxSpiBusManager.h"

//------------------------------------------------------------------------------
//----------------------
// The _BYTE_ACC enables byte-by-byte access for multi-byte variables. This
// MUST be defined when multi-byte variable is stored in big-endian and/or
// address miss-aligned access is prohibited. 

#define _BYTE_ACC

//----------------------
// Read only configuration. This removes writing functions, f_write, f_sync,
// f_unlink, f_mkdir, f_chmod, f_rename and f_getfree. 

//#define _FS_READONLY

//----------------------
// The _FS_MINIMIZE defines minimization level to remove some functions.
//  0: Not minimized.
//  1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod and f_rename are removed.
//  2: f_opendir and f_readdir are removed in addition to level 1. 

#define _FS_MINIMIZE	0

//----------------------
// When _USE_SJIS is defined, Shift-JIS code transparency is enabled, otherwise
// only US-ASCII(7bit) code can be accepted as file/directory name. 

//#define	_USE_SJIS

//------------------------------------------------------------------------------


// File system object structure 
typedef struct _FATFS
{
	unsigned char	fs_type;		// FAT type 
	unsigned char	files;			// Number of files currently opend 
	unsigned char	sects_clust;	        // Sectors per cluster 
	unsigned char	n_fats;			// Number of FAT copies 
	unsigned short	n_rootdir;		// Number of root directory entry 
	unsigned char	winflag;		// win[] dirty flag (1:must be written back) 
	unsigned char	pad1;
	unsigned long	winsect;		// Current sector appearing in the win[] 
	unsigned long	sects_fat;		// Sectors per fat 
	unsigned long	max_clust;		// Maximum cluster# + 1 
	unsigned long	fatbase;		// FAT start sector 
	unsigned long	dirbase;		// Root directory start sector (cluster# for FAT32) 
	unsigned long	database;		// Data start sector 
	unsigned long	last_clust;		// Last allocated cluster 
	unsigned char	win[512];		// Disk access window for Directory/FAT 
} FATFS;

//------------------------------------------------------------------------------
// FAT type signature (FATFS.fs_type)

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


// File attribute bits for directory entry 

#define	AM_RDO	0x01	          // Read only 
#define	AM_HID	0x02	          // Hidden 
#define	AM_SYS	0x04	          // System 
#define	AM_VOL	0x08	          // Volume label 
#define AM_LFN	0x0F	          // LFN entry 
#define AM_DIR	0x10	          // Directory 
#define AM_ARC	0x20	          // Archive 

//------------------------------------------------------------------------------

// Multi-byte word access macros 

#ifdef _BYTE_ACC
#define	LD_WORD(ptr)		(unsigned short)(((unsigned short)*(unsigned char*)((ptr)+1)<<8)|(unsigned short)*(unsigned char*)(ptr))
#define	LD_DWORD(ptr)		(unsigned long)(((unsigned long)*(unsigned char*)((ptr)+3)<<24)|((unsigned long)*(unsigned char*)((ptr)+2)<<16)|((unsigned short)*(unsigned char*)((ptr)+1)<<8)|*(unsigned char*)(ptr))
#define	ST_WORD(ptr,val)	*(unsigned char*)(ptr)=(unsigned char)(val); *(unsigned char*)((ptr)+1)=(unsigned char)((unsigned short)(val)>>8)
#define	ST_DWORD(ptr,val)	*(unsigned char*)(ptr)=(unsigned char)(val); *(unsigned char*)((ptr)+1)=(unsigned char)((unsigned short)(val)>>8); *(unsigned char*)((ptr)+2)=(unsigned char)((unsigned long)(val)>>16); *(unsigned char*)((ptr)+3)=(unsigned char)((unsigned long)(val)>>24)
#else
#define	LD_WORD(ptr)		(unsigned short)(*(unsigned short*)(unsigned char*)(ptr))
#define	LD_DWORD(ptr)		(unsigned long)(*(unsigned long*)(unsigned char*)(ptr))
#define	ST_WORD(ptr,val)	*(unsigned short*)(unsigned char*)(ptr)=(unsigned short)(val)
#define	ST_DWORD(ptr,val)	*(unsigned long*)(unsigned char*)(ptr)=(unsigned long)(val)
#endif

//------------------------------------------------------------------------------
 
   

// FatFs module application interface                
class CxSdFatFs :  public IxFileSystem
{  
 public:  
    // helper class
    class CxFatLock
    {  
     public:  
    
       CxFatLock( FIL* fileData, CxMutex & mutex );
       ~CxFatLock();
       
     private:  
       
       // pointer to the file structure
       FIL* file;
       
       // reference to the mutex  
       CxMutex & int_mutex;
       
       // don't copy this helper class
       CxFatLock& operator=( CxFatLock const & other );   
    };   

   CxSdFatFs( );
   ~CxSdFatFs(){}
   
   static CxSdFatFs & getInstance();

   FRESULT f_open     ( FIL*, const char*, unsigned char );			// Open or create a file 
   FRESULT f_read     ( FIL*, void*, unsigned short, unsigned short* );		// Read file 
   FRESULT f_write    ( FIL*, const void*, unsigned short, unsigned short* );	// Write file 
   FRESULT f_lseek    ( FIL*, unsigned long );					// Seek file pointer   
   FRESULT f_close    ( FIL* );							// Close file    
   
   FRESULT f_opendir  ( DIR*, const char* );				        // Open a directory 
   FRESULT f_readdir  ( DIR*, FILINFO* );					// Read a directory item       
   FRESULT f_stat     ( const char*, FILINFO* );				// Get file status          
   FRESULT f_getfree  ( unsigned long* );					// Get number of free clusters
   FRESULT f_unlink   ( const char* );						// Delete a file or directory 
   FRESULT f_mkdir    ( const char* );						// Create a directory    
   FRESULT f_chmod    ( const char*, unsigned char, unsigned char );		// Change file attriburte 
   FRESULT f_rename   ( const char*, const char* );		                // Rename a file or directory 
   
   FRESULT check_mounted();                                                     // Make Sure that the File System is Valid  
   
 protected:   
      
 private:  
   
   FRESULT f_mountdrv (void);						        // Force initialized the file system          
   FRESULT f_sync     (FIL*);							// Flush cached data of a writing file    
      
   // User defined function to give a current time to fatfs module 
   unsigned long get_fattime(void);                           //31-25: Year(0-127 +1980), 24-21: Month(1-12), 20-16: Day(1-31) 
			                                      // 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) 
   // Change Window Offset  
   bool move_window( unsigned long sector );
   // Get a Cluster Status 
   unsigned long get_cluster( unsigned long clust );
   // Change a Cluster Status  
   bool put_cluster( unsigned long clust, unsigned long val );
   // Remove a Cluster Chain 
   bool remove_chain( unsigned long clust );
   // Stretch or Create a Cluster Chain
   unsigned long create_chain( unsigned long clust );   
   // Get Sector# from Cluster#   
   unsigned long clust2sect( unsigned long clust );   
   // Check File System Type  
   unsigned char check_fs( unsigned long sect );
   // Move Directory Pointer to Next  
   bool next_dir_entry( DIR *scan );  
   // Get File Status from Directory Entry  
   void get_fileinfo( FILINFO *finfo, const unsigned char *dir );  
   // Pick a Paragraph and Create the Name in Format of Directory Entry  
   char make_dirfile( const char **path, char *dirname );   
   // Trace a File Path  
        // Pointer to directory object to return last directory  
        // Pointer to last segment name to return  
        // Full-path string to trace a file or directory  
        // Directory pointer in Win[] to retutn  
   FRESULT trace_path( DIR *scan, char *fn, const char *path, unsigned char **dir ); 
   // Reserve a Directory Entry  
   unsigned char* reserve_direntry( DIR *scan );
      
   // variables             
   CxMutex mFatAccessMutex;     
   
   // bus manger class   
   CxSpiBusManager & mSpiBusManager;
   
   // fat structure
   FATFS internal_fat;
   
   // File R/W buffer 
   unsigned char buffer[512];	        
       
}; typedef CxSdFatFs *pCxSdFatFs;

#endif //_CX_SD_FAT_FS 


