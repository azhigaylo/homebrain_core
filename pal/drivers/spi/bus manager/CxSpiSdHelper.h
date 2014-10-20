#ifndef _CX_SPI_SD_HELPER
#define _CX_SPI_SD_HELPER

//------------------------------------------------------------------------------

#include "FreeRTOS.h"
#include "CxSpiBusHelper.h"

//------------------------------------------------------------------------------

typedef unsigned char	DSTATUS;
typedef unsigned char	DRESULT;

//------------Results of Disk Functions (DRESULT)-------------------------------

#define RES_OK			0		// Successful 
#define	RES_ERROR		1		// R/W Error 
#define	RES_WRPRT		2		// Write Protected
#define	RES_NOTRDY		3		// Not Ready 
#define	RES_PARERR		4		// Invalid Parameter 

//------------Disk Status Bits (DSTATUS)----------------------------------------

#define STA_NOINIT		0x01	        // Drive not initialized 
#define STA_NODISK		0x02	        // No medium in the drive 
#define STA_PROTECT		0x04	        // Write protected 

//------------Command code for disk_ioctrl()------------------------------------

#define GET_SECTORS		1
#define PUT_IDLE_STATE	        2
#define MMC_GET_CSD		10
#define MMC_GET_CID		11
#define MMC_GET_OCR		12
#define ATA_GET_REV		20
#define ATA_GET_MODEL	        21
#define ATA_GET_SN		22

//------------------------------------------------------------------------------
// at here we have to wrote function for SD card
//------------------------------------------------------------------------------


class CxSpiSdHelper 
{ 
 public: 
   
   CxSpiSdHelper( CxSpiBusHelper & busHelper );
   ~CxSpiSdHelper( ){}   

   DSTATUS init_SD( );
   DSTATUS disk_status ();
   DRESULT disk_read( unsigned char *buff, unsigned long sector, unsigned char count );
   DRESULT disk_write( const unsigned char *buff, unsigned long sector, unsigned char count );
   DRESULT disk_ioctl( unsigned char ctrl, void *buff );   
   
   
 private:   
   

   unsigned char wait_ready( );
   unsigned char send_cmd( unsigned char cmd, unsigned long arg );   
   
   char mChipNumber;
   DSTATUS mDiskStatus;
   
   CxSpiBusHelper & mSpiBusHelper; 
   
}; 

typedef CxSpiSdHelper *pCxSpiSdHelper;

#endif /*_CX_SPI_SD_HELPER*/   
