#ifndef _PTYPES_H
#define _PTYPES_H

#include <sys/types.h>

#ifndef int8_t
   typedef signed char        int8_t;
#endif

#ifndef uint8_t
   typedef unsigned char      uint8_t;
#endif

#ifndef int16_t
   typedef signed short       int16_t;
#endif

#ifndef uint16_t
   typedef unsigned short     uint16_t;
#endif

#ifndef int32_t
   typedef signed int         int32_t;
#endif

#ifndef uint32_t
   typedef unsigned int       uint32_t;
#endif

#ifndef int64_t
   //typedef long int           int64_t;
#endif

#endif /* _PTYPES_H */
