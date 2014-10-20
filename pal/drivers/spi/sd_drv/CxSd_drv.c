//------------------------------------------------------------------------------

#include <stdlib.h>

//------------------------------------------------------------------------------

#include "Utils.h"
#include "CxSd_drv.h"

//------------------------------------------------------------------------------
CxSd_drv &sd_drv = CxSd_drv::getInstance();   
//------------------------------------------------------------------------------

CxSd_drv::CxSd_drv( ):
   IxDriver    ( "SD_DRV" )  
  ,SdFatFs     ( CxSdFatFs::getInstance() ) 
  ,pFileSystem ( reinterpret_cast<pIxFileSystem>(&SdFatFs) ) 
  ,bFatState   ( false )   
{

} 

CxSd_drv::~CxSd_drv( )
{

}

CxSd_drv &CxSd_drv::getInstance( )
{
  static CxSd_drv theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxSd_drv::CommandProcessor( TCommand &Command )
{ 
  // command process
  switch( Command.ComID )
  {  
    case C_GET_FAT_INTERFACE : 
    {
      TCommand ComIOsize  = { Command.ConsumerID, Command.SenderID, response, 
                              C_GET_FAT_INTERFACE,  reinterpret_cast<void*>(pFileSystem) };
      SendCommand( &ComIOsize );
      break;
    }     

    default :  {   }    // unknown command 
  }     
}

void CxSd_drv::ThreadProcessor( )
{  
  if( bFatState == true )
  {
    if( FR_OK != SdFatFs.check_mounted() )
    {
      TCommand ComSdNotReady  = { DrvID, ConsumerID, response, C_EVENT_SD_NOT_READY, NULL };
      SendCommand( &ComSdNotReady );
      bFatState = false;
    }  
  } 
  else
  {
    if( FR_OK == SdFatFs.check_mounted() )
    {
      TCommand ComSdReady  = { DrvID, ConsumerID, response, C_EVENT_SD_READY, NULL };
      SendCommand( &ComSdReady );  
      bFatState = true;
    }    
  }          
  task_sleep( 10 );
}

//------------------------------------------------------------------------------



	
