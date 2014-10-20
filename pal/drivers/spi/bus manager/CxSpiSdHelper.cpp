//------------------------------------------------------------------------------

#include "CxSysTimer.h"
#include "CxSpiSdHelper.h"
#include "..\bus manager\CxSpiBusManager.h"

//------------------------------------------------------------------------------
static volatile unsigned char sdControlTimer;			/* 100Hz decrement timer */

void sd_timerproc ()
{
   unsigned char n;
   n = sdControlTimer;						             
   if (n) sdControlTimer = --n;
}

CxSysVirtualTimer SDControlTimer( sd_timerproc, 10, CxSysVirtualTimer::cycle, CxSysVirtualTimer::on );

//------------------------------------------------------------------------------

#define SPI_SD_CS_NUMBER        0  

// MMC/SD command (in SPI) 
#define CMD0	(0x40+0)	// GO_IDLE_STATE 
#define CMD1	(0x40+1)	// SEND_OP_COND 
#define CMD9	(0x40+9)	// SEND_CSD 
#define CMD10	(0x40+10)	// SEND_CID 
#define CMD12	(0x40+12)	// STOP_TRANSMISSION 
#define CMD17	(0x40+17)	// READ_SINGLE_BLOCK 
#define CMD18	(0x40+18)	// READ_MULTIPLE_BLOCK 
#define CMD24	(0x40+24)	// WRITE_BLOCK 
#define CMD25	(0x40+25)	// WRITE_MULTIPLE_BLOCK 
#define CMD58	(0x40+58)	// READ_OCR 

//------------------------------------------------------------------------------

CxSpiSdHelper::CxSpiSdHelper( CxSpiBusHelper & busHelper ):
    mChipNumber   ( SPI_SD_CS_NUMBER ) 
   ,mDiskStatus   ( STA_NOINIT )  
   ,mSpiBusHelper ( busHelper )   
{    

}

//------------------------------------------------------------------------------

DSTATUS CxSpiSdHelper::init_SD( )
{
    //----------------------------------------------------------------------

    // set chip-select-register
    // 8 bits per transfer, CPOL=0, ClockPhase=0, DLYBCT = 0
    mSpiBusHelper.setSpiPropertis( mChipNumber, AT91C_SPI_NCPHA | AT91C_SPI_CSAAT | AT91C_SPI_BITS_8 ) ;    
    mSpiBusHelper.setSpeed( mChipNumber, 0xFE );                 // slow during init    
    mSpiBusHelper.chipSelect( mChipNumber );                     // set PCS for fixed select    
    // Send 20 spi commands with card not selected 
    for( char i=0; i<21; i++ ) 
    {
      mSpiBusHelper.xmit_spi( 0xFF );
    }  
    
    //----------------------------------------------------------------------

    mDiskStatus |= STA_NOINIT;
    if ( !(mDiskStatus & STA_NODISK) ) 
    {
        #if 0
        unsigned char n = 10;		    	                                // Dummy clock 
        do
                mSpiBusHelper.rcvr_spi();
        while (--n);
        #endif
        if( send_cmd(CMD0, 0) == 1 )                                             // Enter Idle state 
        {			                        
            sdControlTimer = 100;				                // Wait for card ready in timeout of 1 sec
            while( sdControlTimer && send_cmd(CMD1, 0) );
            if( sdControlTimer ) mDiskStatus &= ~STA_NOINIT;	                // When device goes ready, clear STA_NOINIT 
    }
        mSpiBusHelper.rcvr_spi( );		                // Idle (Release DO) 
    }

    // max speed
    mSpiBusHelper.setSpeed( mChipNumber, 0x00 );

    return mDiskStatus;
}

DSTATUS CxSpiSdHelper::disk_status( )
{
    return mDiskStatus;
}

DRESULT CxSpiSdHelper::disk_read( unsigned char *buff, unsigned long sector, unsigned char count )
{
    if ( mDiskStatus & STA_NOINIT ) return RES_NOTRDY;
    if ( !count ) return RES_PARERR;

    sector *= 512;		                                                // LBA --> byte address 

    if( count == 1 ) 
    {	
      // READ_SINGLE_BLOCK
      if ( (send_cmd(CMD17, sector) == 0) && mSpiBusHelper.rcvr_datablock(buff, (unsigned char)(512/2)) )
      {  
        count = 0;
      }  
    }
    else 
    {				
      // READ_MULTIPLE_BLOCK
      if ( send_cmd(CMD18, sector) == 0 ) 
      {	
          do 
          {
              if ( !mSpiBusHelper.rcvr_datablock(buff, (unsigned char)(512/2)) ) break;
              buff += 512;
          } 
          while ( --count );
          
          send_cmd( CMD12, 0 );				                //STOP_TRANSMISSION 
      }
    }

    mSpiBusHelper.rcvr_spi();			                //Idle (Release DO)

    return count ? RES_ERROR : RES_OK;  
}

DRESULT CxSpiSdHelper::disk_write( const unsigned char *buff, unsigned long sector, unsigned char count )
{
    if ( mDiskStatus & STA_NOINIT  ) return RES_NOTRDY;
    if ( mDiskStatus & STA_PROTECT ) return RES_WRPRT;
    if ( !count ) return RES_PARERR;

    sector *= 512;		                                                // LBA --> byte address   

    if( count == 1 ) 
    {	
       // Single block write
       if( (send_cmd(CMD24, sector) == 0) && (wait_ready() == 0xFF) )
       {
         if( true == mSpiBusHelper.xmit_datablock(buff, 0xFE) )
         {
           count = 0;
         }  
       }               
    }
    else 
    {
      //Multiple block write
      if( send_cmd(CMD25, sector) == 0 ) 
      {	
          do 
          {
              if( wait_ready() != 0xFF ) break;
              if ( !mSpiBusHelper.xmit_datablock(buff, 0xFC) ) break;
              buff += 512;
          }
          while( --count );

          if( wait_ready() == 0xFF )
          {
            if ( false == mSpiBusHelper.xmit_datablock(0, 0xFD) )
            {
              count = 1;
            }  
          }
          else 
          {
            count = 1;
          }    
       }
    }

    mSpiBusHelper.rcvr_spi();			                // Idle (Release DO) 

    return count ? RES_ERROR : RES_OK;  
}

DRESULT CxSpiSdHelper::disk_ioctl( unsigned char ctrl, void *buff )
{
    DRESULT res;
    unsigned char n, csd[16], *ptr = (unsigned char*)buff;
    unsigned short csm, csize;

    if( mDiskStatus & STA_NOINIT ) return RES_NOTRDY;

    res = RES_ERROR;
    switch( ctrl ) 
    {
        // Get number of sectors on the disk (unsigned long) 
        case GET_SECTORS :	
                if( (send_cmd(CMD9, 0) == 0) && mSpiBusHelper.rcvr_datablock(csd, 16/2) ) 
                {
                   // Calculate disk size
                   csm = 1 << (((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2);
                   csize = ((unsigned short)(csd[8] & 3) >> 6) + (unsigned short)(csd[7] << 2) + ((unsigned short)(csd[6] & 3) << 10) + 1;
                   *(unsigned long*)ptr = (unsigned long)csize * csm;
                   res = RES_OK;
                }
                break;
        // Receive CSD as a data block (16 bytes) 
        case MMC_GET_CSD :	
                if( (send_cmd(CMD9, 0) == 0) && mSpiBusHelper.rcvr_datablock(ptr, 16/2) )
                {  
                   res = RES_OK;
                }   
                break;
        // Receive CID as a data block (16 bytes) 
        case MMC_GET_CID :	
                if( (send_cmd(CMD10, 0) == 0)&& mSpiBusHelper.rcvr_datablock(ptr, 16/2) )
                {
                  res = RES_OK;
                }  
                break;
        // Receive OCR as an R3 resp (4 bytes)
        case MMC_GET_OCR :	
                if( send_cmd(CMD58, 0) == 0 ) 
                {	
                    for( n = 0; n < 4; n++ ) *ptr++ = mSpiBusHelper.rcvr_spi();
                    res = RES_OK;
                }
                break;

        default:
                res = RES_PARERR;
    }

    mSpiBusHelper.rcvr_spi();			                // Idle (Release DO)

    return res;  
}  

//------------------------------------------------------------------------------

unsigned char CxSpiSdHelper::wait_ready( )
{
    unsigned char res;

    sdControlTimer = 50;			                                // Wait for ready in timeout of 500ms 
    mSpiBusHelper.rcvr_spi();
    
    do
    {  
       res = mSpiBusHelper.rcvr_spi();
    }        
    while( (res != 0xFF) && sdControlTimer );
    return res;  
}

unsigned char CxSpiSdHelper::send_cmd( unsigned char cmd, unsigned long arg )
{
    unsigned char n = 10, res;

    if( wait_ready() != 0xFF ) return 0xFF;

    // Send command packet
    mSpiBusHelper.xmit_spi( cmd );				// Command 
    mSpiBusHelper.xmit_spi( (unsigned char)(arg >> 24) );	// Argument[31..24] 
    mSpiBusHelper.xmit_spi( (unsigned char)(arg >> 16) );	// Argument[23..16] 
    mSpiBusHelper.xmit_spi( (unsigned char)(arg >> 8) );	        // Argument[15..8] 
    mSpiBusHelper.xmit_spi( (unsigned char)arg );		// Argument[7..0] 
    mSpiBusHelper.xmit_spi( 0x95 );				// CRC (valid for only CMD0) 

    // Receive command response
    if( cmd == CMD12 )                                                          // Skip a stuff byte when stop reading 
    {
      mSpiBusHelper.rcvr_spi();		        
    }    
    // Wait for a valid response in timeout of 10 attempts
    do
    {  
       res = mSpiBusHelper.rcvr_spi();
    }        
    while( (res & 0x80) && --n );
    
    //Return with the response value
    return res;			
}
