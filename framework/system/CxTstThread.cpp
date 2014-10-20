#include "CxTstThread.h"
#include "PSysI.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

#include "CxSysTimer.h"

//------------------------------------------------------------------------------

// commands
#define   Y_ON          1
#define   Y_OFF         2
#define   G_ON          3
#define   G_OFF         4

//------------------------------------------------------------------------------
CxTstThread &TstThread = CxTstThread::getInstance();   
//------------------------------------------------------------------------------

CxTstThread::CxTstThread( ):
   CxThreadIO( "TST_TASK", "BLINK_TASK" )
  ,IntrvalTimer( this, 1000, CxVTimer::cycle, CxVTimer::on )
  ,flash_on( false )
  ,flash_y ( true  )
  ,flash_g ( false )   
{

} 

CxTstThread::~CxTstThread( )
{

}

CxTstThread &CxTstThread::getInstance( )
{
  static CxTstThread theInstance;
  return theInstance;
}

void CxTstThread::Start()
{ 
  // create thread
  task_run( );
}

//------------------------------------------------------------------------------

void CxTstThread::TimerEventProcessor( unsigned short timerID )
{
   flash_on = true; 
}

//------------------------------------------------------------------------------

void CxTstThread::ThreadProcessor()
{
  switch( LauncherState )
  {    
      case ST_L_UNKNOWN : 
      {         
        LauncherState = ST_L_CONFIG;                                            // put in next state
        break;
      }
      case ST_L_CONFIG :  
      { 
        LauncherState = ST_L_WAIT_DRV_RESP;                                     // put in next state
        break;
      }
      case ST_L_WAIT_DRV_RESP :  
      { 
        LauncherState = ST_L_NORMAL_WORK; 
        break;
      }	
      case ST_L_NORMAL_WORK :  
      {         
        WorkCycle();
        break;
      }	      
      case ST_L_SLEEP :  
      { 
        LauncherState = ST_L_CONFIG;                                            // put in next state
        break;
      }	                              

      default : break;
   }

}

//------------------------------------------------------------------------------
#include <string.h>
//#include "..\pal\drivers\spi\sd_fatfs\CxSdFatFs.h"
//CxSdFatFs &SdFatFs = CxSdFatFs::getInstance();
//FIL file;
char temp_buff[100];
//unsigned char disc_staus;
unsigned short reallly = 0;

void CxTstThread::WorkCycle()
{    
  DBG_SCOPE(CxTstThread, CxTstThread)            
    
  if( flash_on == true )
  {        
    DBG_MSG( ("[N] Sys tick = %l \n\r", tick_counter) ); 
    
    memset( temp_buff, 0, 50 );
    int ID = FileOpen( "/data/new/new.txt", FA_READ | FA_WRITE );
    reallly = FileRead( ID, temp_buff, 30 );
    FileClose( ID );
    
    DBG_MSG( ("[N] = %s \n\r", temp_buff) );               
      
    if( flash_y == true )
    {                  
      TCommand Command1  = { threadID, drvID, request, Y_ON,  NULL  };
      SendCommand( &Command1 ); 
      TCommand Command2  = { threadID, drvID, request, G_OFF,  NULL };
      SendCommand( &Command2 );   
      flash_y = false;
      flash_g = true;     
    }  
    else
    {
      TCommand Command3  = { threadID, drvID, request, G_ON,  NULL  };
      SendCommand( &Command3 ); 
      TCommand Command4  = { threadID, drvID, request, Y_OFF,  NULL };
      SendCommand( &Command4 );  
      flash_y = true;
      flash_g = false;  
    }
    flash_on = false; 
  }  
  task_sleep( 1 );
}

//------------------------------------------------------------------------------

void CxTstThread::SysTimerChecker()
{    
  tick_counter++;
}  

//------------------------------------------------------------------------------
void test_tick_visualization()
{
   TstThread.SysTimerChecker();
}

CxSysVirtualTimer SysTimeTimer( test_tick_visualization, 500, CxSysVirtualTimer::cycle, CxSysVirtualTimer::on );
