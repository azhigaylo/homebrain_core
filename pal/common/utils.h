#ifndef _UTILS
#define _UTILS
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------
#include "ptypes.h"
//-----------------------------System Utilities---------------------------------
void            sleep_mcs( uint64_t delay_in_ms );
void            sleep_s  ( uint64_t delay_in_s  );
//-----------------------------Utilities-Routines-------------------------------
char*           strncpy_m ( char* e_str, char* s_str, int32_t dest_size );
void*           memset_m  ( void* ptr,   int value,   int32_t num, int32_t dest_size );
uint32_t        strlen_m  ( const char*string, uint32_t reasonvalue );
void*           memcpy_m  ( void* destination, const void* source, int32_t num, int32_t dest_size );

bool            mod_memcpy  ( char*to, char*from, unsigned short datasize, unsigned short sizeofto );

bool            mod_strstr  ( char*string1, char*string2, unsigned short len1, unsigned short len2 );
char*           mod_strchr  ( char* string1, char symbol, unsigned short sStringSize );
char*           mod_strrchr ( char* string1, char symbol, unsigned short sStringSize );
bool            mod_strcmp  ( char *s1, char *s2);

unsigned short  GenWfrom2B  ( char B_h, char B_l );
unsigned long   GenLfrom2W  ( unsigned short W_h, unsigned short W_l );
unsigned char   LOW         ( unsigned short celoe );
unsigned char   HIGH        ( unsigned short celoe );
char            TurnBitInByte( char Byte );
void            LShiftArray ( char*buff,unsigned short b_size,unsigned short shift );
void            revertbuff  ( char*dest,unsigned short chng_size );
void            inversbuff  ( char*dest,unsigned short chng_size );

unsigned short  CRC16b      ( char *Buff, unsigned short Count, unsigned char base );
unsigned short  CRC16_T     ( char *Buff, unsigned short Count );
//unsigned short  CRC16_T     ( unsigned char *Buff, unsigned short Count, unsigned short in_crc );
unsigned short  CRC16_T_0   ( char *Buff, unsigned short Count );
//-----------------------------ModBus utylites----------------------------------
unsigned short  getWordFromMbReg   ( unsigned short ReadFromMB );
float getFloatFromTwoMbReg( unsigned short registerMB_1, unsigned short registerMB_2 );

#endif /*_UTILS*/
