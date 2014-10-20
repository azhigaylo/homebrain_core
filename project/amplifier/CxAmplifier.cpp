#include "PSysI.h"
#include "CxAmplifier.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxAmplifier &audioMUX= CxAmplifier::getInstance();

//------------------------------------------------------------------------------

CxAmplifier::CxAmplifier():
  CxLogDevice( "DEV_MUX" )

{    
   setNotification( CxEvent::EVENT_DP_NEW_VALUE );
}

CxAmplifier &CxAmplifier::getInstance()
{
  static CxAmplifier theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

bool CxAmplifier::processEvent( pTEvent pEvent )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  // data conteyner event processing  
  if( pEvent->eventType == CxEvent::EVENT_DP_NEW_VALUE )
  {    
    pTDPOINT pDPOINT = reinterpret_cast<pTDPOINT>(pEvent->eventData); 
    DBG_MSG( ("[M] CxAmplifier: DP-%b = %w \n\r", pDPOINT->number, pDPOINT->value ) );
    
    bool result = true;
    
    switch( pDPOINT->number ) 
    {
       case 0 :
       {
         result = SetAudioMute( pDPOINT->value );
         break;
       }  
       case 1:
       {
         result = SetAudioGain( pDPOINT->value );
         break;         
       }  
       case 2:
       {
         result = SetAudioLoudness( pDPOINT->value );
         break;         
       }         
       case 3:
       {
         result = SetAudioChannel( pDPOINT->value );
         break;         
       }  
       case 4:
       {
         result = SetAudioLevel( pDPOINT->value ); 
         break;         
       }  
       case 5:
       {
         result = SetAudioBass( pDPOINT->value );
         break;         
       }  
       case 6:
       {
         result = SetAudioTreble( pDPOINT->value );
         break;         
       }     
       case 7:
       {
         result = SetAudioAttLF( pDPOINT->value );
         break;         
       }                  
       case 8:
       {
         result = SetAudioAttRF( pDPOINT->value );
         break;         
       }   
       case 9:
       {
         result = SetAudioAttLR( pDPOINT->value );
         break;         
       }   
       case 10:
       {
         result = SetAudioAttRR( pDPOINT->value );
         break;         
       }       
       default :  {   }    // unknown command     
    }  
    
    if( result == false )
    {  
      // reset port 
      DBG_MSG( ("[M] CxAmplifier: write failure") );   
    }  

    return true;
  }

  // driver event processing  
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  
  if( pEvent->eventType == intsrnalECB.TxEvent )
  {
     DBG_MSG( ("[M] CxAmplifier: write end \n\r") );   
     return true;
   }   
  
  return false;
}

bool CxAmplifier::SetAudioMute( char value )   
{        
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_MUTE, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;
  }
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioMute \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioGain( char value )                
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_CHANNEL, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;       
  }  
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioGain \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioLoudness( char value )                  
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_CHANNEL, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;       
  } 
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioLoudness \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioChannel( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_CHANNEL, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;          
  }  
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioChannel \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioLevel( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_LEVEL, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;         
  }  
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioLevel \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioBass( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_BASS, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;         
  }  
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioBass \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioTreble( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_TREBLE, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;       
  }   
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioTreble \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioAttLF( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_ATT_LF, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;       
  }  
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioAttLF \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioAttRF( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_ATT_RF, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;         
  } 
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioAttRF \n\r") );   
  }  
  
  return result; 
}

bool CxAmplifier::SetAudioAttLR( char value )
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_ATT_LR, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;     
  } 
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioAttLR \n\r") );   
  }  
  
  return result;
}

bool CxAmplifier::SetAudioAttRR( char value )   
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )   
          
  bool result = true;
  TAmpCommand AmpCommand = { C_MUX_ATT_RR, value };

  if( -1 == FileWrite( PORT, reinterpret_cast<char*>(&AmpCommand), sizeof(AmpCommand) ) )
  {
    result = false;      
  } 
  else
  {
    DBG_MSG( ("[M] CxAmplifier: SetAudioAttRR \n\r") );   
  }  
  
  return result; 
}

//------------------------------------------------------------------------------

void CxAmplifier::Start()
{
  DBG_SCOPE( CxAmplifier, CxAmplifier )

  // set notification on Tx finished event
  ECB intsrnalECB;
  GetCommEvent( PORT, &intsrnalECB );
  setNotification( intsrnalECB.TxEvent );
  
  //
  DBG_MSG( ("[M] Audio MUX task has been runned \n\r") );  
}
