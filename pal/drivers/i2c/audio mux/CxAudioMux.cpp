#include "CxAudioMux.h"
#include "Utils.h"

//------------------------------------------------------------------------------
CxAudioMux &mux_drv = CxAudioMux::getInstance();   
//------------------------------------------------------------------------------

CxAudioMux::CxAudioMux( ):
   IxDriver         ( "MUX_DRV" )
  ,inputBufferSize  ( 10   )
  ,outputBufferSize ( 10   ) 
  ,pInputBuffer     ( NULL )
  ,pOutputBuffer    ( NULL )  
  ,InputLength      ( 0    )  
  ,I2CJobManager    ( CxI2CJobManager::getInstance() ) 
  ,address          ( 0x44 )  
{
  mod_memset( reinterpret_cast<char*>(&currentJob), 0, sizeof(currentJob), sizeof(currentJob) );
  mod_memset( reinterpret_cast<char*>(&AudioMuxControl), 0, sizeof(AudioMuxControl), sizeof(AudioMuxControl) );
  
} 

CxAudioMux::~CxAudioMux( )
{

}

CxAudioMux &CxAudioMux::getInstance( )
{
  static CxAudioMux theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxAudioMux::CommandProcessor( TCommand &Command )
{ 
  // command process
  switch ( Command.ComID )
  {
   case C_SET_IN_BUFFER :
      {
        pInputBuffer = *( reinterpret_cast<char**>(Command.Container) );  
        break;
      }  
   case C_SET_OUT_BUFFER :
      {
        pOutputBuffer = *( reinterpret_cast<char**>(Command.Container) );
        break;
      }
   case C_SET_DCB  : 
      {      
        break;
      }        
    case C_SEND_DATA : 
      {
        if( NULL != pOutputBuffer )
        {  
           unsigned short dataLength = *(reinterpret_cast<unsigned short*>( Command.Container ));
           LoadOutputBuffer( dataLength );
           StartTxD();
        }
        else
        {
          // !!!
        }  
        break;
      }   
    case C_GET_IO_SIZE : 
      {
        TCommand ComIOsize  = { Command.ConsumerID, Command.SenderID, request, C_GET_IO_SIZE,  reinterpret_cast<void*>(&outputBufferSize) };
        SendCommand( &ComIOsize );
        break;
      }     
    case C_STOP : 
      {
        StopTxD();
        break;
      }          
      
    default :  {   }    // unknown command 
  }     
}

void CxAudioMux::ThreadProcessor( )
{
  if( I2CJobManager.WasJobFinished( DrvID ) )
  {
    pTI2CRecBlock pI2CRecBlock = I2CJobManager.GetJobResult( DrvID );
     
    if( NULL != pI2CRecBlock )
     {
       InputLength = pI2CRecBlock->rdLength;  
       mod_memcpy( pOutputBuffer, pI2CRecBlock->rdData, InputLength, inputBufferSize );
       if( InputLength > 0)
       {
          TCommand ComIOsize  = { DrvID, ConsumerID, request, C_NEED_DATA_PROCESS, &InputLength };
          SendCommand( &ComIOsize );
       }    
     }  
     else
     {
        TCommand ComIOsize  = { DrvID, ConsumerID, response, C_SEND_DATA_END, NULL };
        SendCommand( &ComIOsize );       
     }           
  }
}

//------------------------------------------------------------------------------

void CxAudioMux::LoadOutputBuffer( unsigned short buf_size )
{
  pTAudioMuxCommand pAudioMuxCommand = reinterpret_cast<pTAudioMuxCommand>(pOutputBuffer);
  
  switch( pAudioMuxCommand->commID )
  {
     case C_MUX_MUTE :
     {
       SetAudioMute( pAudioMuxCommand->value );
       break;
     }  
     case C_MUX_CHANNEL:
     {
       SelectAudioChannel( pAudioMuxCommand->value );
       break;         
     }  
     case C_MUX_LEVEL:
     {
       SetAudioLevel( pAudioMuxCommand->value ); 
       break;         
     }  
     case C_MUX_BASS:
     {
       SetAudioBass( pAudioMuxCommand->value );
       break;         
     }  
     case C_MUX_TREBLE:
     {
       SetAudioTreble( pAudioMuxCommand->value );
       break;         
     }     
     case C_MUX_ATT_LF:
     {
       SetAudioAttLF( pAudioMuxCommand->value );
       break;         
     }                  
     case C_MUX_ATT_RF:
     {
       SetAudioAttRF( pAudioMuxCommand->value );
       break;         
     }   
     case C_MUX_ATT_LR:
     {
       SetAudioAttLR( pAudioMuxCommand->value );
       break;         
     }   
     case C_MUX_ATT_RR:
     {
       SetAudioAttRR( pAudioMuxCommand->value );
       break;         
     }       
     default :  {   }    // unknown command     
  }   
}

void CxAudioMux::SetAudioMute( char value )
{
   AudioMuxControl.mute = value;
  
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 4;
   currentJob.rdLength  = 0;
   
   if( value == 1 )
   {
     // mute "ON"
     currentJob.wrtData[0] = MUX_ATT_LF | 0x1F;   
     currentJob.wrtData[1] = MUX_ATT_RF | 0x1F;   
     currentJob.wrtData[2] = MUX_ATT_LR | 0x1F;    
     currentJob.wrtData[3] = MUX_ATT_RR | 0x1F;         
   }  
   else
   {
     // mute "OFF" and loan previous value
     currentJob.wrtData[0] = MUX_ATT_LF | AudioMuxControl.attLF;   
     currentJob.wrtData[1] = MUX_ATT_RF | AudioMuxControl.attRF;   
     currentJob.wrtData[2] = MUX_ATT_LR | AudioMuxControl.attLR;    
     currentJob.wrtData[3] = MUX_ATT_RR | AudioMuxControl.attRR;    
   }  
}

void CxAudioMux::SelectAudioChannel( char value )
{
   AudioMuxControl.channel = value;
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   
   
   currentJob.wrtData[0] = value | MUX_SWITCH;   
}

void CxAudioMux::SetAudioLevel( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   
   
   // level limitation
   if( value > 64 ) 
   {
     value = 64;
   }  
   // store valu in internal var
   AudioMuxControl.level = value;
   
   // address preset
   value |= MUX_VOLUME;
   
   // put value in job buffer
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioBass( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   
   
   // level limitation
   if( (value > 7) && (value < 16) ) 
   {
     value = 0x08 + 15 - value;
   }      
   // store valu in internal var   
   AudioMuxControl.bass = value;

   // address preset
   value |= MUX_BASS;
      
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioTreble( char value )
{
   AudioMuxControl.treble = value;
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   

   // level limitation
   if( (value > 7) && (value < 16) ) 
   {
     value = 0x08 + 15 - value;
   }   
   
   // store valu in internal var   
   AudioMuxControl.treble = value;

   // address preset
   value |= MUX_TREBLE;
   
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioAttLF( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   

   // level limitation
   if( value > 32 ) 
   {
     value = 32;
   }  
   
   // store valu in internal var
   AudioMuxControl.attLF = value;
   
   // address preset
   value |= MUX_ATT_LF;
   
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioAttRF( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   

   // level limitation
   if( value > 32 ) 
   {
     value = 32;
   }  
   
   // store valu in internal var
   AudioMuxControl.attRF = value;   
   
   // address preset
   value |= MUX_ATT_RF;
   
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioAttLR( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   

   // level limitation
   if( value > 32 ) 
   {
     value = 32;
   }  
   
   // store valu in internal var
   AudioMuxControl.attLR = value;   
      
   // address preset
   value |= MUX_ATT_LR;
   
   currentJob.wrtData[0] = value;   
}

void CxAudioMux::SetAudioAttRR( char value )
{
   // fill up job content 
   currentJob.driverID  = DrvID;
   currentJob.nodeAddr  = address;
   currentJob.opType    = opType_write;
   currentJob.wrtLength = 1;
   currentJob.rdLength  = 0;   

   // level limitation
   if( value > 32 ) 
   {
     value = 32;
   }  
   
   // store valu in internal var
   AudioMuxControl.attRR = value;   
         
   // address preset
   value |= MUX_ATT_RR;
   
   currentJob.wrtData[0] = value;   
}

//------------------------------------------------------------------------------

void CxAudioMux::StartTxD()
{
  I2CJobManager.SetJob( currentJob );
}    

void CxAudioMux::StopTxD()
{
   I2CJobManager.StopAllJob( );
}

//------------------------------------------------------------------------------
