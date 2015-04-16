#include "PSysI.h"
#include "MBusDB.h"
#include "CxBusSlave.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxBusSlave &mbSlave = CxBusSlave::getInstance();

//------------------------------------------------------------------------------

CxBusSlave::CxBusSlave():
   IxRunnable     ( "MBS_TASK"   )
  ,CxLogDevice    ( "DEV_MBS"    )
  ,newRequestWasResived ( false  )  
  ,BusHelper( MBTABLE )  
{    
  mod_memset( ioBuffer, 0, IO_BUFFER_SIZE, IO_BUFFER_SIZE );  
  mod_memset( reinterpret_cast<char*>(MBTABLE), 0, MB_TABEL_SIZE, MB_TABEL_SIZE );
  
  BusHelper.setDRouteTableItem( 0,  0  );     // amp mute
  BusHelper.setDRouteTableItem( 2,  1  );     // amp gain 
  BusHelper.setDRouteTableItem( 4,  2  );     // amp loudness
  BusHelper.setDRouteTableItem( 6,  3  );     // amp channel
  BusHelper.setDRouteTableItem( 8,  4  );     // amp volume  
  BusHelper.setDRouteTableItem( 10, 5  );     // amp bass
  BusHelper.setDRouteTableItem( 12, 6  );     // amp treble
  
  BusHelper.setDRouteTableItem( 14, 7  );     // amp att LF
  BusHelper.setDRouteTableItem( 16, 8  );     // amp att RF
  BusHelper.setDRouteTableItem( 18, 9  );     // amp att LR
  BusHelper.setDRouteTableItem( 20, 10 );     // amp att RR
}

CxBusSlave &CxBusSlave::getInstance()
{
  static CxBusSlave theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxBusSlave::MBusReadProcessor()
{
   DBG_SCOPE( CxBusSlave, CxBusSlave )
  
   pTMREQ  pMREQ  = reinterpret_cast<pTMREQ>( ioBuffer );  
   pTMRESP pMRESP = reinterpret_cast<pTMRESP>( ioBuffer );  
   
   unsigned short start_reg = GenWfrom2B( pMREQ->start_reg_hi, pMREQ->start_reg_low );
   unsigned short numb_reg  = GenWfrom2B( pMREQ->numb_reg_hi, pMREQ->numb_reg_low   );
   
   pMRESP->Header.counter = numb_reg * sizeof(unsigned short);
   
   // prepare data for response
   unsigned short posOut = 0;
   for( unsigned short posIn = start_reg; posIn < start_reg+numb_reg; posIn++, posOut++ )
   {
     if( posOut < (sizeof(ioBuffer) - sizeof(TMRESPHeader) - sizeof(short))/2 )
     {  
        pMRESP->OutputBuf[posOut] = MBTABLE[posIn];   
     }
     else
     {
       // !!       
       DBG_MSG( ("[M] CxBusSlave: read register out of renge \n\r") );
     }  
   } 
   // CRC calculation
   pMRESP->OutputBuf[posOut] = CRC16_T( ioBuffer, sizeof(TMRESPHeader) + numb_reg*sizeof(unsigned short) );
   
   // send responce for host processor
   if( -1 == FileWrite( PORT, ioBuffer, sizeof(TMRESPHeader) + numb_reg*sizeof(unsigned short) + sizeof(short) ) )
   {
      // reset port 
      if( -1 != FileClose( PORT ) ) PORT = -1;                       
   }  
}

//------------------------------------------------------------------------------

void CxBusSlave::MBusWriteProcessor()
{
   DBG_SCOPE( CxBusSlave, CxBusSlave )
    
   pTMWRREG pMWRREG  = reinterpret_cast<pTMWRREG>( ioBuffer );  

   unsigned short start_reg = GenWfrom2B( pMWRREG->start_reg_hi, pMWRREG->start_reg_low );

   // prepare data for response
   if( start_reg < MB_TABEL_SIZE )
   {  
      MBTABLE[start_reg] = pMWRREG->REG;   
      BusHelper.CheckAndChangeInternalDataPoint( static_cast<char>(start_reg) );
   }   
   else
   {
     // !!
     DBG_MSG( ("[M] CxBusSlave: write register out of renge \n\r") );
   }  
 
   // send responce for host processor
   if( -1 == FileWrite( PORT, ioBuffer, sizeof(TMWRREG) ) )
   {
      // reset port 
      if( -1 != FileClose( PORT ) ) PORT = -1;                        
   }
}    

//------------------------------------------------------------------------------

void CxBusSlave::TaskProcessor()
{
  DBG_SCOPE( CxBusSlave, CxBusSlave )
  
  pTMREQ  pMREQ  = reinterpret_cast<pTMREQ>( ioBuffer );
  
  if( enable_operation == true )
  {      
    if( PORT != -1 )
    {
      if( newRequestWasResived == true )
      {        
        if( pMREQ->address == MY_ADDRESS )
        {  
          switch( pMREQ->command )
          {
              case CMD_MB_RREG : 
              {
                MBusReadProcessor(); 
                DBG_MSG( ("[M] CxBusSlave: read \n\r") ); 
                break;
              } 
              case CMD_MB_WREG : 
              {
                DBG_MSG( ("[M] CxBusSlave: write \n\r") ); 
                MBusWriteProcessor(); 
                break;
              }             
              default   : 
              {
                DBG_MSG( ("[M] commant is not in list \n\r") );   
                break;
              }                  
          }  
        }                        
        newRequestWasResived = false;
      }                
    }  
    else
    {
      //If port has been lost or not opened - try to reconnect one         
      if( PORT == -1 )
      {
         SwitchOn();   
      }    
    }           
  }
}

//------------------------------------------------------------------------------

bool CxBusSlave::processEvent( pTEvent pEvent )
{
  DBG_SCOPE( CxBusSlave, CxBusSlave )
  
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  
  if( pEvent->eventType == intsrnalECB.RxEvent )
  {     
     int dataLength = FileRead( PORT, ioBuffer, IO_BUFFER_SIZE );
 
     if( 0 == CRC16_T( ioBuffer, dataLength ) )
     {       
        newRequestWasResived = true;
     }
     else
     {
       DBG_MSG( ("[M] CxBusSlave: crc error \n\r") );
     }
     
     return true;
   }   
   return false;    
}

//------------------------------------------------------------------------------

void CxBusSlave::Start()
{
  DBG_SCOPE( CxBusSlave, CxBusSlave )

  // set notification on Tx finished event
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  setNotification( intsrnalECB.RxEvent );
  
  //
  DBG_MSG( ("[M] ModBUS task has been runned \n\r") );  
  // start task
  task_run( );  
}
