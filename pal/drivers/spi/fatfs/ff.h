/*--------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.03a       (C)ChaN, 2006
/---------------------------------------------------------------------------/
/ FatFs module is an experimenal project to implement FAT file system to
/ cheap microcontrollers. This is a free software and is opened for education,
/ research and development under license policy of following trems.
/
/  Copyright (C) 2006, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is no warranty.
/ * You can use, modify and/or redistribute it for personal, non-profit or
/   profit use without any restriction under your responsibility.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------*/

#ifndef _FATFS

//#error Don't forget to change these options.

#define _BYTE_ACC
/* The _BYTE_ACC enables byte-by-byte access for multi-byte variables. This
/  MUST be defined when multi-byte variable is stored in big-endian and/or
/  address miss-aligned access is prohibited. */

//#define _FS_READONLY
/* Read only configuration. This removes writing functions, f_write, f_sync,
/  f_unlink, f_mkdir, f_chmod, f_rename and f_getfree. */

#define _FS_MINIMIZE	0
/* The _FS_MINIMIZE defines minimization level to remove some functions.
/  0: Not minimized.
/  1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod and f_rename are removed.
/  2: f_opendir and f_readdir are removed in addition to level 1. */

//#define	_USE_SJIS
/* When _USE_SJIS is defined, Shift-JIS code transparency is enabled, otherwise
/  only US-ASCII(7bit) code can be accepted as file/directory name. */


//#include "integer.h"


/* Result type for fatfs application interface */
typedef unsigned char	FRESULT;


/* File system object structure */
typedef struct _FATFS {
	unsigned char	fs_type;		/* FAT type */
	unsigned char	files;			/* Number of files currently opend */
	unsigned char	sects_clust;	/* Sectors per cluster */
	unsigned char	n_fats;			/* Number of FAT copies */
	unsigned short	n_rootdir;		/* Number of root directory entry */
	unsigned char	winflag;		/* win[] dirty flag (1:must be written back) */
	unsigned char	pad1;
	unsigned long	winsect;		/* Current sector appearing in the win[] */
	unsigned long	sects_fat;		/* Sectors per fat */
	unsigned long	max_clust;		/* Maximum cluster# + 1 */
	unsigned long	fatbase;		/* FAT start sector */
	unsigned long	dirbase;		/* Root directory start sector (cluster# for FAT32) */
	unsigned long	database;		/* Data start sector */
	unsigned long	last_clust;		/* Last allocated cluster */
	unsigned char	win[512];		/* Disk access window for Directory/FAT */
} FATFS;


/* Directory object structure */
typedef struct _DIR {
	unsigned long	sclust;		/* Start cluster */
	unsigned long	clust;		/* Current cluster */
	unsigned long	sect;		/* Current sector */
	unsigned short	index;		/* Current index */
} DIR;


/* File object structure */
typedef struct _FIL {
	unsigned long	fptr;			/* File R/W pointer */
	unsigned long	fsize;			/* File size */
	unsigned long	org_clust;		/* File start cluster */
	unsigned long	curr_clust;		/* Current cluster */
	unsigned long	curr_sect;		/* Current sector */
#ifndef _FS_READONLY
	unsigned long	dir_sect;		/* Sector containing the directory entry */
	unsigned char*	dir_ptr;		/* Ponter to the directory entry in the window */
#endif
	unsigned char	flag;			/* File status flags */
	unsigned char	sect_clust;		/* Left sectors in cluster */
	unsigned char	buffer[512];	/* File R/W buffer */
} FIL;


/* File status structure */
typedef struct _FILINFO {
	unsigned long fsize;			/* Size */
	unsigned short fdate;				/* Date */
	unsigned short ftime;				/* Time */
	unsigned char fattrib;			/* Attribute */
	char fname[8+1+3+1];	/* Name (8.3 format) */
} FILINFO;



/*-----------------------------------------------------*/
/* FatFs module application interface                  */

extern FATFS *FatFs;	/* Pointer to active file system object */

FRESULT f_open (FIL*, const char*, unsigned char);			/* Open or create a file */
FRESULT f_read (FIL*, void*, unsigned short, unsigned short*);			/* Read file */
FRESULT f_close (FIL*);								/* Close file */
FRESULT f_lseek (FIL*, unsigned long);						/* Seek file pointer */
FRESULT f_opendir (DIR*, const char*);				/* Open a directory */
FRESULT f_readdir (DIR*, FILINFO*);					/* Read a directory item */
FRESULT f_stat (const char*, FILINFO*);				/* Get file status */
FRESULT f_getfree (unsigned long*);							/* Get number of free clusters */
FRESULT f_mountdrv (void);							/* Force initialized the file system */
FRESULT f_write (FIL*, const void*, unsigned short, unsigned short*);	/* Write file */
FRESULT f_sync (FIL*);								/* Flush cached data of a writing file */
FRESULT f_unlink (const char*);						/* Delete a file or directory */
FRESULT	f_mkdir (const char*);						/* Create a directory */
FRESULT f_chmod (const char*, unsigned char, unsigned char);			/* Change file attriburte */
FRESULT f_rename (const char*, const char*);		/* Rename a file or directory */


/* User defined function to give a current time to fatfs module */

unsigned long get_fattime(void);	/* 31-25: Year(0-127 +1980), 24-21: Month(1-12), 20-16: Day(1-31) */
							/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */



/* File function return code (FRESULT) */

#define	FR_OK						0
#define	FR_NOT_READY				1
#define	FR_NO_FILE					2
#define	FR_NO_PATH					3
#define	FR_INVALID_NAME				4
#define	FR_DENIED					5
#define	FR_DISK_FULL				6
#define	FR_RW_ERROR					7
#define	FR_INCORRECT_DISK_CHANGE	8
#define	FR_WRITE_PROTECTED			9
#define	FR_NOT_ENABLED				10
#define	FR_NO_FILESYSTEM			11


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#ifndef _FS_READONLY
#define	FA_WRITE			0x02
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif
#define FA__ERROR			0x80


/* FAT type signature (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */



/* Multi-byte word access macros  */

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


#define _FATFS
#endif
