/*----------------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2006                        */
/*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------

#include "spi_master_drv.h"
#include "..\bus manager\CxSpiSdHelper.h"

CxSpiSdHelper SpiSdHelper;

/*-----------------------*/
/* Initialize Disk Drive */

DSTATUS disk_initialize ()
{
        return SpiSdHelper.init_SD( );
}



/*----------------*/
/* Shutdown       */


DSTATUS disk_shutdown ()
{
	return 0;
}



/*--------------------*/
/* Return Disk Status */

DSTATUS disk_status ()
{
        return SpiSdHelper.disk_status ();
}



/*----------------*/
/* Read Sector(s) */

DRESULT disk_read (
	unsigned char *buff,			/* Data buffer to store read data */
	unsigned long sector,		        /* Sector number (LBA) */
	unsigned char count			/* Sector count (1..255) */
)
{
        return SpiSdHelper.disk_read( buff, sector, count );
}



/*-----------------*/
/* Write Sector(s) */

#ifndef _READONLY
DRESULT disk_write (
	const unsigned char *buff,	/* Data to be written */
	unsigned long sector,		/* Sector number (LBA) */
	unsigned char count			/* Sector count (1..255) */
)
{
        return SpiSdHelper.disk_write( buff, sector, count );
}
#endif



/*--------------------------*/
/* Miscellaneous Functions  */

DRESULT disk_ioctl (
	unsigned char ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
        return SpiSdHelper.disk_ioctl(ctrl, buff);
}

unsigned long get_fattime ()
{

	return	((2006UL-1980) << 25)	// Year = 2006
			| (2UL << 21)			// Month = Feb
			| (9UL << 16)			// Day = 9
			| (22U << 11)			// Hour = 22
			| (30U << 5)			// Min = 30
			| (0U >> 1)				// Sec = 0
			;

}
