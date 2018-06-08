#ifndef _DATADB
#define _DATADB

//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "provider/CxDataProvider.h"
//------------------------------------------------------------------------------

#define GetDiscretPoint    1
#define GetAnalogPoint     2
#define SetDiscretPoint    3
#define SetAnalogPoint     4
#define NotifyDiscretPoint 5
#define NotifyAnalogPoint  6

//------------------------------------------------------------------------------

#pragma pack(push, 1)

   struct THeader
   {
      uint8_t  data_size;      // overall package size
      uint8_t  cmd;
      uint16_t start_point;
      uint16_t number_point;
   };

   struct TClientRequest
   {
      THeader header;
      union
      {
         TDPOINT digital;
         TAPOINT analog;
      }point;
   };

   // serve for response or notification
   struct TResponse
   {
      THeader header;
      union
      {
         TDPOINT digital[d_point_total];
         TAPOINT analog[a_point_total];
      }array;
   };

#pragma pack(pop)

//------------------------------------------------------------------------------

#endif /*_DATADB*/
