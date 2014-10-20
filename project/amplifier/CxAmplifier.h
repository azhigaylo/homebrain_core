#ifndef _CX_AMPLIFIER
#define _CX_AMPLIFIER

//------------------------------------------------------------------------------

#include "..\framework\devctrl\CxLogDevice.h"
#include "..\framework\eventpool\IxEventConsumer.h"
#include "..\framework\provider\CxDataProvider.h"

//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TAmpCommand
{  
  unsigned char commID;              // 1/2/3/4/5
  unsigned char value;               // 
};
typedef TAmpCommand *pTAmpCommand;
#pragma pack ( )

//------------------------------------------------------------------------------

class CxAmplifier : public CxLogDevice, public IxEventConsumer
{
 public: 
   
   enum TAudioMuxCommand
   {
      C_MUX_MUTE = 1,
      C_MUX_CHANNEL,
      C_MUX_LEVEL,
      C_MUX_BASS,
      C_MUX_TREBLE,
      C_MUX_ATT_LF,
      C_MUX_ATT_RF,
      C_MUX_ATT_LR,
      C_MUX_ATT_RR      
   };
   
    static CxAmplifier& getInstance( );    
    
    virtual void  Start();

 private:   

    bool SetAudioMute       ( char value );             
    bool SetAudioGain       ( char value );                
    bool SetAudioLoudness   ( char value );                  
    bool SetAudioChannel    ( char value );
    bool SetAudioLevel      ( char value );
    bool SetAudioBass       ( char value );
    bool SetAudioTreble     ( char value );
    bool SetAudioAttLF      ( char value );
    bool SetAudioAttRF      ( char value );
    bool SetAudioAttLR      ( char value );
    bool SetAudioAttRR      ( char value );                
    
    CxAmplifier();
    ~CxAmplifier( ){}

    virtual bool processEvent( pTEvent pEvent );                                // form IxEventConsumer
}; 

typedef CxAmplifier *pCxAmplifier;

#endif /*_CX_AMPLIFIER*/   
