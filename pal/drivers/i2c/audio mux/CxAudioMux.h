#ifndef _CX_USART_1_DRV
#define _CX_USART_1_DRV


//------------------------------------------------------------------------------

#include "IxDriver.h"
#include "..\job manager\CxI2CJobManager.h"

//------------------------------------------------------------------------------

// Volume matrix
#define MUX_VOLUME        0x00

// Attenuator matrix
#define MUX_ATT_LF        0x80            // LF
#define MUX_ATT_RF        0xA0            // RF
#define MUX_ATT_LR        0xC0            // LR
#define MUX_ATT_RR        0xE0            // RR

// Audio switch
#define MUX_SWITCH        0x40            

// Audio BASS
#define MUX_BASS          0x60  

// Audio TREBLE
#define MUX_TREBLE        0x70   

//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TAudioMuxControl
{  
  unsigned char mute;                // 1 - mute / 2 - demute
  unsigned char channel;             // 0 - first / ... / 3 - last
  unsigned char level;               // 0 .. 63
  unsigned char bass;                // 0 .. 15 
  unsigned char treble;              // 0 .. 15 
  unsigned char attLF;               // 0 .. 31 
  unsigned char attRF;               // 0 .. 31 
  unsigned char attLR;               // 0 .. 31 
  unsigned char attRR;               // 0 .. 31 

};  
typedef TAudioMuxControl *pTAudioMuxControl;

struct TAudioMuxCommand
{  
  unsigned char commID;              // 1/2/3/4/5
  unsigned char value;               // 
};
typedef TAudioMuxCommand *pTAudioMuxCommand;
#pragma pack ( )

//------------------------------------------------------------------------------

class CxAudioMux : public IxDriver
{
  public:  
     
     static CxAudioMux& getInstance();

  protected:

   enum TSerialDrvCommand
   {
      C_SET_DCB = 1,
      C_SET_IN_BUFFER,
      C_SET_OUT_BUFFER,
      C_SEND_DATA,
      C_NEED_DATA_PROCESS,
      C_GET_IO_SIZE,            
      C_SEND_DATA_END,
      C_STOP
   };

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

  protected:

  private:       
    
    // function's   
    CxAudioMux();       
    ~CxAudioMux();
     
    virtual void CommandProcessor( TCommand &Command );
    virtual void ThreadProcessor ( );
    
    
    void LoadOutputBuffer   ( unsigned short buf_size );  
    
    void SetAudioMute       ( char value );                 // 1 - On / 0 - Off
    void SelectAudioChannel ( char value );
    void SetAudioLevel      ( char value );
    void SetAudioBass       ( char value );
    void SetAudioTreble     ( char value );
    void SetAudioAttLF      ( char value );
    void SetAudioAttRF      ( char value );
    void SetAudioAttLR      ( char value );
    void SetAudioAttRR      ( char value );    
    
    void StartTxD( );    
    void StopTxD ( );

    //--------------------------------------------------------------------------
    
    // used for "C_GET_IO_SIZE" command
    unsigned short inputBufferSize;                                             // IO buffer size
    unsigned short outputBufferSize;
    
    // used for processing "C_SET_IN_BUFFER" and "C_SET_OUT_BUFFER" 
    char* pInputBuffer;                                                         // pointer on IO buffer in top level serial connection  
    char* pOutputBuffer; 

    // used for "C_NEED_DATA_PROCESS"
    unsigned short InputLength;  
    
    // job manager instanse
    CxI2CJobManager & I2CJobManager;
    
    // 
    TI2CJob currentJob;
    
    // mux control structure
    TAudioMuxControl AudioMuxControl;
    
    // my addess
    const char address;

 }; typedef CxAudioMux *pCxAudioMux;
 
//------------------------------------------------------------------------------

#endif // _CX_SYSTEM

