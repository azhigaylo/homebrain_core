#ifndef _MODBUS
#define _MODBUS

//-------------------------mod bus element's------------------------------------
enum TModBusCommand
{
    //for parani ESD OEM module
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
   char address; 
   char command; 
   char start_reg_hi; 
   char start_reg_low; 
   char numb_reg_hi; 
   char numb_reg_low; 
   char numb_b;  
   unsigned short CRC;
}; 
typedef TMREQ *pTMREQ;         
  
//request structure 
struct TMWRREG
{
   char address; 
   char command; 
   char start_reg_hi; 
   char start_reg_low;   
   unsigned short REG; 
   unsigned short CRC;
}; typedef TMWRREG *pTMWRREG;
               
struct TMRESPHeader
{
   char address; 
   char command; 
   char counter;
}; 

struct TMRESP
{
   TMRESPHeader Header;
   unsigned short OutputBuf[50];
}; 
typedef TMRESP *pTMRESP;  

//------------------------------------------------------------------------------
// IO Bit
struct TBITFIELD
{
   unsigned short BITFIELD;   
}; 
typedef TBITFIELD *pTBITFIELD;

struct TIO_BIT
{
  unsigned char  DevType;
  unsigned char  LogicType;
  unsigned short NPoint;
  unsigned short NIOBit;
};
typedef TIO_BIT *pTIO_BIT;  

#pragma pack ( )

#endif /*_MODBUS*/   
