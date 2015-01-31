#ifndef _CX_EVENTDEFINITION
#define _CX_EVENTDEFINITION

//------------------------------------------------------------------------------
//----------------base event calss----------------------------------------------
//------------------------------------------------------------------------------

namespace event_pool
{
   enum eEventType
   {
      EVENT_DUMMY = 0,
      EVENT_POWER_ON,
      EVENT_POWER_OFF,
      // UART
      EVENT_UART1_Rx,
      EVENT_UART2_Rx,
      EVENT_UART1_Tx,
      EVENT_UART2_Tx,
      // DATA Conteiner
      EVENT_DP_NEW_VALUE,
      EVENT_AP_NEW_VALUE,
      EVENT_DP_NEW_STATUS,
      EVENT_AP_NEW_STATUS,
      // FFS mounted
      EVENT_FFS_MOUNTED,
      EVENT_FFS_UNMOUNTED,
      EVENT_SS
   };
}

#endif // _CX_EVENTDEFINITION

