#ifndef _CX_SD_DRV
#define _CX_SD_DRV


//------------------------------------------------------------------------------

#include "IxDriver.h"
#include "IxFileSystem.h"
#include "..\pal\drivers\spi\sd_fatfs\CxSdFatFs.h"

//------------------------------------------------------------------------------

class CxSd_drv : public IxDriver
{
  public:  
     
    static CxSd_drv& getInstance();

  protected:

    enum TFatDrvCommand
    {
      C_GET_FAT_INTERFACE = 1,
      C_EVENT_SD_NOT_READY,
      C_EVENT_SD_READY
    };

  protected:

  private:       
    
    // function's   
    CxSd_drv();       
    ~CxSd_drv();
     
    virtual void CommandProcessor( TCommand &Command );
    virtual void ThreadProcessor ( );

    // fat pointer
    CxSdFatFs & SdFatFs;
    
    // fat interface
    pIxFileSystem pFileSystem;
    
    // fat state
    bool bFatState;                                                             // true - mounted / false - no init
    
 }; typedef CxSd_drv *pCxSd_drv;
 
//------------------------------------------------------------------------------

#endif // _CX_SYSTEM

