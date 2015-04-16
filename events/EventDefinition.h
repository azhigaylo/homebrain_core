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
      // DATA Conteiner
      EVENT_DP_NEW_VALUE,
      EVENT_AP_NEW_VALUE,
      EVENT_DP_NEW_STATUS,
      EVENT_AP_NEW_STATUS,
      // FFS mounted
      EVENT_FFS_MOUNTED,
      EVENT_FFS_UNMOUNTED,
      EVENT_DATA_CONNECTED,
      EVENT_LAST_EVENT
   };
}

#endif // _CX_EVENTDEFINITION

