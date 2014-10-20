#ifndef _CX_SPI_BUS_MANAGER
#define _CX_SPI_BUS_MANAGER

//------------------------------------------------------------------------------

#include "Utils.h"
#include "CxMutex.h"
#include "CxSpiBusHelper.h"
#include "CxSpiSdHelper.h"

//------------------------------------------------------------------------------

class CxSpiBusManager
{  
 public:  

   CxSpiBusManager( );
   ~CxSpiBusManager(){}
   
   static CxSpiBusManager & getInstance();
      
   DSTATUS sd_disk_initialize ();
   DSTATUS sd_disk_shutdown ();
   DSTATUS sd_disk_status ();
   DRESULT sd_disk_read (unsigned char*, unsigned long, unsigned char);
   DRESULT sd_disk_write (const unsigned char*, unsigned long, unsigned char);
   DRESULT sd_disk_ioctl (unsigned char, void*);

 protected:   
   
   //helper class   
   CxSpiBusHelper mSpiBusHelper;    
   CxSpiSdHelper  mSpiSdHelper;    
      
 private:  
   
   CxMutex mSpiControlMutex;
     
}; typedef CxSpiBusManager *pCxSpiBusManager;

#endif /*_CX_SPI_BUS_MANAGER*/   
