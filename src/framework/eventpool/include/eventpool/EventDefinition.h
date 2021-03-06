/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

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

      EVENT_DP_NEW_VALUE,
      EVENT_DP_NEW_STATUS,
      EVENT_AP_NEW_VALUE,
      EVENT_AP_NEW_STATUS,

      EVENT_LAST_EVENT
   };
}

#endif // _CX_EVENTDEFINITION

