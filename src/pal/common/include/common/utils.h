#ifndef _UTILS
#define _UTILS
//------------------------------------------------------------------------------
#include <stdarg.h>
//------------------------------------------------------------------------------
#include "ptypes.h"
//-----------------------------System Utilities---------------------------------
void     sleep_mcs( uint32_t delay_in_ms );
void     sleep_s  ( uint32_t delay_in_s  );

//------------------------------------------------------------------------------
uint16_t GenWfrom2B(uint8_t B_h, uint8_t B_l);
uint32_t GenLfrom2W(uint16_t W_h, uint16_t W_l);
uint8_t  LOW(uint16_t celoe);
uint8_t  HIGH(uint16_t celoe);

//-----------------------------ModBus-------------------------------------------
uint16_t getWordFromMbReg     ( uint16_t registerMB );
uint32_t getLongFromTwoMbReg  ( uint16_t registerMB_1, uint16_t registerMB_2 );
float    getFloatFromTwoMbReg ( uint16_t registerMB_1, uint16_t registerMB_2 );

//------------------------------------------------------------------------------
uint16_t CRC16b      ( char *Buff, uint16_t Count, uint8_t base );
uint16_t CRC16_T     ( char *Buff, uint16_t Count );
uint16_t CRC16_T_0   ( char *Buff, uint16_t Count );

#endif /*_UTILS*/
