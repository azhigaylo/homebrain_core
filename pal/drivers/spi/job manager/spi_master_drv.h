/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.02    (C)ChaN, 2006
/-----------------------------------------------------------------------*/

#ifndef _DISKIF

#include "..\bus manager\CxSpiSdHelper.h"

DSTATUS disk_initialize ();
DSTATUS disk_shutdown ();
DSTATUS disk_status ();
DRESULT disk_read (unsigned char*, unsigned long, unsigned char);
DRESULT disk_write (const unsigned char*, unsigned long, unsigned char);
DRESULT disk_ioctl (unsigned char, void*);
void	disk_timerproc ();

#define _DISKIF
#endif
