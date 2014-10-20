#ifndef _CX_EVENTIX
#define _CX_EVENTIX

//------------------------------------------------------------------------------
//----------------base event calss----------------------------------------------
//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TEvent
{
   unsigned short eventType;
   void *eventData;
}; typedef TEvent *pTEvent;
#pragma pack ( )

class CxEvent  
{
  public:
    enum EventType
    {
       EVENT_DUMMY = 0,
       // KEYPAD
       EVENT_POWER_ON,
       EVENT_POWER_OFF,
       // UART
       EVENT_USART1_Rx,
       EVENT_USART2_Rx,
       EVENT_AMUX_Rx,
       EVENT_USART1_Tx,
       EVENT_USART2_Tx,
       EVENT_AMUX_Tx,
       // DATA Conteiner
       EVENT_DP_NEW_VALUE,       
       EVENT_AP_NEW_VALUE,       
       EVENT_TP_NEW_VALUE, 
       EVENT_DP_NEW_STATUS,       
       EVENT_AP_NEW_STATUS,       
       EVENT_TP_NEW_STATUS,  
       // FFS mounted
       EVENT_FFS_MOUNTED,  
       EVENT_FFS_UNMOUNTED,  
       EVENT_SS
    };
    CxEvent(); 
    virtual ~CxEvent();
    
  protected:
     TEvent Event;   
     
}; typedef CxEvent *pCxEvent; 

#endif // _CX_EVENTIX

