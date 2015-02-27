#ifndef _MODBUS
#define _MODBUS

//------------------------------------------------------------------------------
#include "ptypes.h"

//-------------------------mod bus element's------------------------------------
enum TModBusCommand
{
    CMD_MB_RDOUT   = 0x01,          // read discret output
    CMD_MB_RDINP   = 0x02,          // read discret input
    CMD_MB_RREG    = 0x03,          // read array of MB registers 
    CMD_MB_RAINP   = 0x04,          // read analog input
    CMD_MB_WDOUT   = 0x05,          // write discret output
    CMD_MB_WREG    = 0x06,          // write MB register 
    CMD_MB_WARRREG = 0x10,          // write array of MB registers  
    CMD_MB_NULL
};

#pragma pack ( 1 )
struct TMREQ
{
   uint8_t  address; 
   uint8_t  command; 
   uint8_t  start_reg_hi; 
   uint8_t  start_reg_low; 
   uint8_t  numb_reg_hi; 
   uint8_t  numb_reg_low; 
   uint16_t CRC;
}; 
typedef TMREQ *pTMREQ;         
  
//request structure 
struct TMWRREG
{
   uint8_t  address; 
   uint8_t  command; 
   uint8_t  start_reg_hi; 
   uint8_t  start_reg_low;   
   uint16_t REG; 
   uint16_t CRC;
}; typedef TMWRREG *pTMWRREG;
               
struct TMRESPHeader
{
   uint8_t address; 
   uint8_t command; 
   uint8_t counter;
}; 

struct TMRESP
{
   TMRESPHeader Header;
   uint16_t     OutputBuf[200];
}; 
typedef TMRESP *pTMRESP;  

// IO Bit
struct TBITFIELD
{
   uint16_t BITFIELD;   
}; 
typedef TBITFIELD *pTBITFIELD;

struct TIO_BIT
{
  uint8_t  DevType;
  uint8_t  LogicType;
  uint16_t NPoint;
  uint16_t NIOBit;
};
typedef TIO_BIT *pTIO_BIT;  

#pragma pack ( )

//------------------------------------------------------------------------------

uint16_t ConvertMBint(uint16_t ReadFromMB);

#endif /*_MODBUS*/   
