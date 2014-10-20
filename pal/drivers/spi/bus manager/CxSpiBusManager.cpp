//------------------------------------------------------------------------------

#include "CxSpiBusManager.h"

//------------------------------------------------------------------------------

CxSpiBusManager::CxSpiBusManager( ):
    mSpiControlMutex ( )  
   ,mSpiBusHelper    ( )  
   ,mSpiSdHelper     ( mSpiBusHelper )  

{
  // common SPI bus initialization 
  mSpiBusHelper.init_spi(); 
}

CxSpiBusManager &CxSpiBusManager::getInstance( )
{
  static CxSpiBusManager theInstance;
  return theInstance;
}

//---------------SD disk management function------------------------------------

// Initialize Disk Drive 
DSTATUS CxSpiBusManager::sd_disk_initialize ()
{
   mSpiControlMutex.take();
      DSTATUS result = mSpiSdHelper.init_SD( );
   mSpiControlMutex.give();
   
   return result;
}

// Shutdown
DSTATUS CxSpiBusManager::sd_disk_shutdown ()
{
   DSTATUS result = 0;
   
   return result;
}

// Return Disk Status 
DSTATUS CxSpiBusManager::sd_disk_status ()
{
   mSpiControlMutex.take();
      DSTATUS result = mSpiSdHelper.disk_status ();
   mSpiControlMutex.give();   
   
   return result;
}

// Read Sector(s) 
DRESULT CxSpiBusManager::sd_disk_read( unsigned char *buff, unsigned long sector, unsigned char count )
{
   mSpiControlMutex.take();
      DSTATUS result = mSpiSdHelper.disk_read( buff, sector, count );
   mSpiControlMutex.give();
   
   return result;
}

// Write Sector(s) 
DRESULT CxSpiBusManager::sd_disk_write( const unsigned char *buff, unsigned long sector, unsigned char count )
{
   mSpiControlMutex.take();
      DSTATUS result = mSpiSdHelper.disk_write( buff, sector, count );
   mSpiControlMutex.give();  
   
   return result;
}

// Miscellaneous Functions 
DRESULT CxSpiBusManager::sd_disk_ioctl( unsigned char ctrl, void *buff )
{
   mSpiControlMutex.take();
      DSTATUS result = mSpiSdHelper.disk_ioctl( ctrl, buff );
   mSpiControlMutex.give();   
   
   return result;
}
