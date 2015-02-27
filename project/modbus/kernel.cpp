#include "kernel.h"

//------share address location--------------
__no_init volatile TTIME           TIME    @ 0x7FF8;      prtTTIME pTIME = (prtTTIME)&TIME.Calibration;
__no_init volatile TIO_UART        COM3_IO @ 0x9CB8;      prtTIO_UART pCOM3_IO = (prtTIO_UART)&COM3_IO.cOPack_trans;      
__no_init volatile TIO_UART        COM4_IO @ 0xA488;      prtTIO_UART pCOM4_IO = (prtTIO_UART)&COM4_IO.cOPack_trans; 
__no_init volatile TDATA_BLOCK     M_BLOCK @ 0xADE8;      ptrTDATA_BLOCK pM_BLOCK = (ptrTDATA_BLOCK)&M_BLOCK.cNew_press; 
__no_init volatile TCONTROL_BLOCK  C_BLOCK @ 0xADFC;      
__no_init volatile TMASTER_SETUP   MASTER_BLOCK @ 0xAE06;
__no_init volatile TVIDEO_BUFFER   VIDEO_BLOCK @ 0xB040;  ptrTVIDEO_BUFFER pVIDEO_BUFFER = (ptrTVIDEO_BUFFER)&VIDEO_BLOCK.sVBUFsize;
__no_init volatile TFLGS_BUFFER    FLGS    @ 0xBFF0;        
__no_init volatile TINT_BUFFER     ExtINT  @ 0xBFFE;  

//------internal address location-----------
__no_init volatile TSysFlags       SysFlags; 
__no_init volatile TProcessFlags   ProcFlags;  
__eeprom __no_init TSTATDTA STATDTA;
  
C_VLCD VLCD(pVIDEO_BUFFER, sizeof(VIDEO_BLOCK.VBuf)); C_VLCD_ptr pVLCD = &VLCD;
C_Flash FLASH; C_Flash_ptr pFLASH = &FLASH;
CUART_1_Slave COM1(9600,none); CUART_BASE_ptr pCOM1 = &COM1;   
CUART_2_Slave COM2(9600,none); CUART_BASE_ptr pCOM2 = &COM2;
CUART_extern  COM3(9600,none,pCOM3_IO,(ptrTCOM_SETUP)&MASTER_BLOCK.COM1_SET); CUART_BASE_ptr pCOM3 = &COM3;
CUART_extern  COM4(9600,none,pCOM4_IO,(ptrTCOM_SETUP)&MASTER_BLOCK.COM2_SET); CUART_BASE_ptr pCOM4 = &COM4;
C_KEY_DEVICE  KEYPAD; 
TDATA_BLOCK   DATA;     
// line definition
C_Slist lSubTest(PortProcessor);
C_Slist lRedrwWind(WinRedrawProcessor);
C_Slist lPadProcess(PadProcessor);
C_Slist lDataScan(DataScan);
C_Slist lWDTr(WDTreset);
C_Slist lBlinkSvd(BlinkSvd);
C_Slist lOffLigthLCD(OffLigthLCD);
C_Slist lWorkAlgoritm(UserAlgoritm);
// set line processor
CTimer timer10ms(LineProcessor, 10, continuous, on);
// mode terminator (NORMAL/TERMINAL/RECFILE)
char SMODE = NORMAL;
// file list
TFileListStrc FilesList;
// points
TAPOINT TABEL_APOINT[apoint_number]; 
TDPOINT TABEL_DPOINT[dpoint_number]; 
TBITFIELD TABEL_IBIT[bit_inp_number/16]; 
TBITFIELD TABEL_OBIT[bit_out_number/16]; 

TBITFIELD MTABEL_OBIT[bit_out_number/16]; 
// code succession
TC_CODE_ptr pC_CODE1 = NULL;
TC_CODE_ptr pC_CODE2 = NULL;
TC_CODE_ptr pC_CODE3 = NULL;
TC_CODE_ptr pC_CODE4 = NULL;
TC_CODE_ptr pC_CODE5 = NULL;
// io_bit structure
pTIOBIT pIOBITDATA   = NULL;

//------------------work with shadow line list------------------------------  

C_Slist_ptr GetActivLine()
{
  C_Slist_ptr pLine;
  if(LineList.LineCounter != 0){
    pLine = LineList.pFirstLine;
    for(char obj=0;obj<LineList.LineCounter;obj++){             
     if(pLine->NeedProcess) return(pLine);
     if(!pLine->NextLine)break; 
     pLine= pLine->NextLine;               
    }         
  } 
  return(NULL);    
}
  
void LineProcess()  
{
  C_Slist_ptr pLine;
  pLine = GetActivLine();
  pLine->LineGo();
}
  
void Sleep(unsigned long tick)
{
  if (tick==0) return;
  unsigned long StrtTime = TIMETICK;
 
  while(true){
    if(TIMETICK-StrtTime>tick) return;
    LineProcess();    
  }   
}

void LineProcessor()
{
  C_Slist_ptr pLine;
  if(LineList.LineCounter != 0){
    pLine = LineList.pFirstLine;
    for(char obj=0;obj<LineList.LineCounter;obj++){             
     pLine->LineControl();
     if(!pLine->NextLine)break; 
     pLine= pLine->NextLine;               
    }         
  } 
}

//-------------------Utilities-Routines-----------------------------------
void SetUp()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
{
   //-------port definition----------------
   PORTA = 0xFF; DDRA  = 0xFF;        //def port A
   PORTB = 0xFE; DDRB  = 0x17;        //def port B
   PORTC = 0xFF; DDRC  = 0xFF;        //def port C    
   PORTD = 0x88; DDRD  = 0xFB;        //def port D
   PORTE = 0x03; DDRE  = 0x0E;        //def port E
   PORTF = 0x00; DDRF  = 0x00;        //def port F     
   PORTG = 0x1F; DDRG  = 0x17;        //def port G        
   //set external interrupt
   EICRB.ISC4 = 0x00;
   EIMSK.INT4 = true;       
}

void SimpleDelay(unsigned long ms)
{
  while(ms){
    WDTreset();
    __delay_cycles(FREQUENCY/1000 - 6);
    ms--; 
  }
}

void InitWatchDogTimer(char Delay)
{
    WDTCR = (1<<WDCE)|(1<<WDE);
    WDTCR = (1<<WDE)|(Delay<<WDP0);
}

void WDTreset()
 {
   PORTD &=~Strobe;
   __delay_cycles(4);
   PORTD |=Strobe;   
 }
 
void OnExtSvd()
{
  C_BLOCK.SvdControl = true;
} 
 
void OffExtSvd()
{
  C_BLOCK.SvdControl = false;
} 

void OnLcdLight()
{
  C_BLOCK.cLightOn = true;
} 
 
void OffLcdLight()
{
  C_BLOCK.cLightOn = false;
} 

void OnLcd()
{
  C_BLOCK.cLCDControl = true;
} 
 
void OffLcd()
{
  C_BLOCK.cLCDControl = false;
} 

void BeginUpdate()
{
  FLGS.DataLatch = BUSY;
}

void EndUpdate()
{
  FLGS.DataLatch = FREE;
}

void BlinkSvd()
{
  static char v=0; 
  // blink svd
  if(!v){OnExtSvd(); v=true;}else{OffExtSvd(); v=false;}
  lBlinkSvd.LineSleep(500);  
} 

void DataScan()
{
  DATA.fTerm = M_BLOCK.fTerm; 
  DATA.fU5V  = M_BLOCK.fU5V;
  DATA.fU24v = M_BLOCK.fU24v;
  lDataScan.LineSleep(500);    
}

void OffLigthLCD()
{
  OffLcdLight();
  lOffLigthLCD.LineSleep(10000);  
} 
    
//--------------------------------------------------------------------------------------------
unsigned int CStackKontrol()
{  
  unsigned int   S_free = 0;
  unsigned long  C_BEG = __segment_begin("CSTACK");
  unsigned long  C_END = __segment_end("CSTACK");
  unsigned char* C_Pointer = (unsigned char *)__segment_begin("CSTACK"); 
  for(unsigned int s_p=0; s_p<(unsigned int)(C_END-C_BEG); s_p++)   
    if(*(C_Pointer + s_p)!='$'){S_free = s_p ;break;}   
  return(S_free);
} 

unsigned int RStackKontrol()
{  
  unsigned int   S_free = 0;
  unsigned long  R_BEG = __segment_begin("RSTACK");
  unsigned long  R_END = __segment_end("RSTACK");
  unsigned char* R_Pointer = (unsigned char *)__segment_begin("RSTACK"); 
  for(unsigned int s_p=0; s_p<(unsigned int)(R_END - R_BEG); s_p++)   
    if(*(R_Pointer + s_p)!='$'){S_free = s_p ;break;}   
  return(S_free);
} 

unsigned int HeapKontrol()
{
  unsigned int s = 16;
  for (;;) {
    void *p = malloc(s);
    if(p){
     free(p); 
     s+=16;} else return(s-16);
  }
}

void PresetStack()
{   
  memset((char*)__segment_begin("CSTACK"), '$', __segment_end("CSTACK")-__segment_begin("CSTACK"));    
  memset((char*)__segment_begin("RSTACK"), '$', (__segment_end("RSTACK")-__segment_begin("RSTACK"))-10);      
  memset((char*)__segment_begin("HEAP"), '$', __segment_end("HEAP")-__segment_begin("HEAP"));    
} 

//-----------------------------work with RAM------------------------------------
// RAM - Test & Clear
bool RAM_TST(char *ExtRAM_Base, char *ExtRAM_Top)
{
   char b; char *p; 
   // Test Ext RAM
   b = 0xE3;
   for (p=ExtRAM_Base; p<ExtRAM_Top; p++){ *p = b; b-=0x37; }
   b = 0xE3;
   for (p=ExtRAM_Base; p<ExtRAM_Top; p++){ 
     if (*p != b){   
       for (p=ExtRAM_Base; p<ExtRAM_Top; p++) *p=0; // Ext RAM Clear
       return(false);
     }  
     b-=0x37;
   } 
   
  for (p=ExtRAM_Base; p<ExtRAM_Top; p++) *p=0; // Ext RAM Clear  
  return(true); 
}

// External RAM - Initialization, Test & Clear
void CPU_Init(char WaitStates, char *ExtRAM_Base, char *ExtRAM_Top)
{
  //setup port
  SetUp();
  //clear error flag's & programm flag's
  memset((unsigned char*)(&SysFlags), 0, sizeof(SysFlags));
  memset((unsigned char*)(&ProcFlags), 0, sizeof(ProcFlags));
  //inicialisation memory 
  MCUCR.SRE = true;
  if (WaitStates&1) MCUCR.SRW10 = true;
  if (WaitStates&2) XMCRA.SRW11 = true;
  XMCRB.XMBK = false;      // Ext Mem Bus Keeper Enable
  // Pull-ups for Ext Mem Lines
  PORTA=PORTC=0xFF; PORTG|=0x07;PORTC&=0x7F;  
  if(!RAM_TST(ExtRAM_Base, ExtRAM_Top))SysFlags.error_memory = true;
   else SysFlags.error_memory = false;
}

// Share RAM - Initialization, Test & Clear
void ShareRAM_tst(char *ShrRAM_Base, char *ShrRAM_Top)
{
  if(!RAM_TST(ShrRAM_Base, ShrRAM_Top))SysFlags.error_share_memory = true;
   else SysFlags.error_share_memory = false;
}

//--------------------------------------------------------------------------------------------
//word from byte
unsigned short GenWfrom2B(char B_h, char B_l)
{
  return(((unsigned short)B_h<<8)|((unsigned short)B_l));  
}

//Long from word
unsigned long GenLfrom2W(unsigned short W_h, unsigned short W_l)
{
  return(((unsigned long)W_h<<16)|((unsigned long)W_l));  
}
//round float value - 4 dig after point
float f_round(float r_value)
{
  float tmp_value1 = r_value*10000;
  long  tmp_value2 = (long)tmp_value1;
  float tmp_value3 = tmp_value2/10000;
  return(tmp_value3);
}

//function return LSB of word           
unsigned char LOW(unsigned short celoe)
{   
   union{
     struct{ unsigned char Low; unsigned char High;}HL; 
     unsigned short Dbyte;     
   }UN;      
   UN.Dbyte = celoe;                      
   return(UN.HL.Low);
}

// function return MSB of word           
unsigned char  HIGH(unsigned short celoe)
{  
   union{
     struct{ unsigned char Low; unsigned char High;}HL;
     unsigned short Dbyte;     
   }UN;      
   UN.Dbyte = celoe;                      
   return(UN.HL.High);
}

//turn bit in byte
char RekolbasB(char Byte)
 {
   unsigned char rez=0;
   for(unsigned char i=0;i<8;i++){
     if((unsigned char)(0x80>>i) & Byte)rez |= (unsigned char)((0x01)<<i);      
   } 
   return(rez);  
 }
 
//turn bit in byte
unsigned long OblikLongConvert(unsigned long data)
{ 
  union{ struct{char d1; char d2; char d3; char d4;}HL;
     unsigned long ldata;     
   }UN;   
  UN.ldata = data;
  char tmp = UN.HL.d1; UN.HL.d1 = UN.HL.d4; UN.HL.d4 = tmp; 
       tmp = UN.HL.d2; UN.HL.d2 = UN.HL.d3; UN.HL.d3 = tmp; 
  return(UN.ldata);
}

void LShiftArray(char*buff,unsigned short b_size,unsigned short shift)
{ 
  b_size--;
  for(unsigned short i=0; i<shift;i++){
   memcpy(buff, (buff+1),b_size); *(buff+b_size) = 0;
 }  
}
 
//-------------------------------------------------------------------------
// CRC table
static __flash unsigned char CRC_Hi[] = {
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
};

static __flash unsigned char CRC_Lo[] = {
 0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
 0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
 0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
 0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
 0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
 0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
 0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
 0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
 0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
 0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
 0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
 0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
 0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
 0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
 0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
 0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40,
};

unsigned int CRC16b(unsigned char *Buff, unsigned int Count, unsigned char base)
{
  unsigned char ah = base;
  unsigned char al = base;
  unsigned char bl;
  
  if(Count<3)return (0x000F);
  while (Count) {
    bl = *Buff ^ ah;
    ah = al ^ CRC_Hi[bl];
    al = CRC_Lo[bl];
    Buff++; Count--;
  }
  return (unsigned int)(al<<8) | ah;
}

unsigned int CRC16_T(unsigned char *Buff, unsigned int Count)
{
  return CRC16b(Buff, Count, 0xFF);
}

unsigned int  CRC16_T_0(unsigned char *Buff, unsigned int Count)
{
  return CRC16b(Buff, Count, 0);
}
//return hash code of string 
unsigned short Gethash(char*string)
{
  unsigned short str_hash = 0;
  char i=0;
  while(*(string+i)){  
    str_hash+=*(string+i++);
  }
  return(str_hash);
}

//set port mode
void SetPortMode(CUART_BASE_ptr pCOM, char MODE)  
{  
  switch (MODE){
    case UNUSE : {pCOM->SetMODE(UNUSE); break;}  
    case USER  : {pCOM->SetMODE(USER); break;}   
    case USO   : {pCOM->SetMODE(USO); break;}   
    case HOST  : {pCOM->SetMODE(HOST); break;}  
    default    : {pCOM->SetMODE(HOST);}       
  }      
}

//set port mode
CUART_BASE_ptr GetPortPTR(char N_PORT)  
{  
  switch (N_PORT){
    case 1  : {return(pCOM1);}  
    case 2  : {return(pCOM2);}   
    case 3  : {return(pCOM3);}   
    case 4  : {return(pCOM4);}  
    default : {break;}       
  }      
  return(NULL);
}

void SetTime(char iHour,char iMin,char iSec)
{
  pTIME->Calibration |=0x80;
  pTIME->Sec = (iSec/10)<<4 | (0x0F&(iSec-(iSec/10)*10));
  pTIME->Min = (iMin/10)<<4 | (0x0F&(iMin-(iMin/10)*10));
  pTIME->Hour = (iHour/10)<<4 | (0x0F&(iHour-(iHour/10)*10));
  pTIME->Calibration &=0x7F;
}

void SetData(char iYear,char iMonth,char iData)
{
  pTIME->Calibration |=0x80;
  pTIME->Year  = (iYear/10)<<4 | (0x0F&(iYear-(iYear/10)*10));
  pTIME->Month = (iMonth/10)<<4 | (0x0F&(iMonth-(iMonth/10)*10));
  pTIME->Data  = (iData/10)<<4 | (0x0F&(iData-(iData/10)*10));
  pTIME->Calibration &=0x7F; 
}

void SetPort(char Nport, unsigned long ComSpeed, char Parity, char N_stbit)
{  
  switch (Nport){       
    case 1 : {STATDTA.COM1.lCOMspeed = ComSpeed; 
              if(Parity=='E') STATDTA.COM1.cCOMparity = even;
                else STATDTA.COM1.cCOMparity = none;
              STATDTA.COM1.cCOMbitnumb = N_stbit;    
              break;
             }   
    case 2 : {STATDTA.COM2.lCOMspeed = ComSpeed; 
              if(Parity=='E') STATDTA.COM2.cCOMparity = even;
                else STATDTA.COM2.cCOMparity = none;
              STATDTA.COM2.cCOMbitnumb = N_stbit;    
              break;
             }   
    case 3 : {STATDTA.COM3.lCOMspeed = ComSpeed; 
              if(Parity=='E') STATDTA.COM3.cCOMparity = even;
                else STATDTA.COM3.cCOMparity = none;
              STATDTA.COM3.cCOMbitnumb = N_stbit;    
              break;
             }       
    case 4 : {STATDTA.COM4.lCOMspeed = ComSpeed; 
              if(Parity=='E') STATDTA.COM4.cCOMparity = even;
                else STATDTA.COM4.cCOMparity = none;
              STATDTA.COM4.cCOMbitnumb = N_stbit;    
              break;
             }             
    default   : {break;}
  }  


    // setport 1 9600 E 1
}

//-------------------------teminal command processing----------------
void FormatFlash(CUART_BASE_ptr pCOM)
{
  char need_size=30;
  char *buf = new char[need_size]; memset(buf,0,need_size);
    char lens = sprintf(buf, "\n\r start flash format \n\r");
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    VLCD.TextOut(buf,lens,8);
    //clr all flash
    for(unsigned short sec=0;sec<all_sec;sec++){
      memset(buf,0,need_size);
      VLCD.SetXY(10, VLCD.PosY()); VLCD.TextOut(buf,lens,8);
      char lens = sprintf(buf, "\r sector %i ",sec);
      VLCD.SetXY(10, VLCD.PosY()); VLCD.TextOut(buf,lens,8);
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
      WDTreset();
      pFLASH->CLRsec(sec);
    }  
    memset(buf,0,need_size);
    lens = sprintf(buf, "\n\r create Sblock"); VLCD.TextOut(buf,lens,8);
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    memset(buf,0,need_size);
    lens = sprintf(buf, "\n\r create ROOT \n\r"); VLCD.TextOut(buf,lens,8);   
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
  delete[] buf; 
  //create MBR
  pTSBLK MBR = new TSBLK;
    MBR->Signature   = mFAT_OK;
    MBR->TotalSector = all_sec;
    MBR->BusySector  = dstrt_page;         // 0 - 40
    MBR->LastFreeSector = dstrt_page; 
    MBR->StrtRoot = root_strt;
    MBR->CRC = CRC16_T((unsigned char*)(&MBR), sizeof(TSBLK)-2);
    pFLASH->WDATA((char*)MBR, sizeof(TSBLK),boot_page,0);       
  delete MBR; 
  //create ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    ROOT->Signature   = 0xAAAA;
    ROOT->TotalRecord = 0;
    ROOT->BlkNumber   = 1;
    ROOT->NextBlkPtr  = 0;
    ROOT->BlkCRC = CRC16_T((unsigned char*)(&ROOT), sizeof(TROOTCAT)-2);   
    pFLASH->WDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt); 
  delete ROOT;   
}

void DIR(CUART_BASE_ptr pCOM)
{ 
  //tst MBR&ROOT signature 
  pTSBLK MBR = new TSBLK;
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR; return;}
  delete MBR; 
  //create ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
    if(ROOT->Signature != 0xAAAA){delete ROOT; return;}
    char rec_nmb = ROOT->TotalRecord;
  delete ROOT;
    
  pTFileRecord FREC = new TFileRecord;
  unsigned short n_rec=0;
  for(unsigned short n_real_rec=0;n_real_rec<rec_nmb;n_real_rec++,n_rec++){     
    unsigned short sector = (sizeof(TFileRecord)*n_rec + sizeof(TROOTCAT))/size_sec+root_page;
    unsigned short offset = sizeof(TFileRecord)*n_rec%size_sec + sizeof(TROOTCAT);
    pFLASH->RDATA((char*)FREC, sizeof(TFileRecord),sector,offset);
    if(FREC->FileName[0]!= 0){
      char need_size=30;
      char *buf = new char[need_size]; memset(buf,0,need_size);
        FREC->FileName[20]=0;        
        char lens = sprintf(buf,"\n\r %s ",(char*)FREC->FileName); VLCD.TextOut(buf,lens,8);         
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);
        lens = sprintf(buf, "%ld byte",(long)FREC->FileSize);            
        VLCD.SetXY(100, VLCD.PosY()); VLCD.TextOut(buf,lens,8);              
      delete[] buf; 
    }else{n_real_rec--;}               
  }
  delete FREC;    
   
  return;
}

void InitArhiv(CUART_BASE_ptr pCOM)
{
  char a_file[12]; memset(a_file,0,sizeof(a_file)); sprintf(a_file, "AParh.arh");  
  
  TFilePoint FilePoint = FileExist(a_file);       // file exists analise
  if(FilePoint.sector || FilePoint.offset){DeleteFile(a_file);}   
  pTFileHndl pFileHndl = CreateFile(a_file);

  // creat new zero arhive file
  if(pFileHndl){
   char *Z_file = new char[size_data_sec];  
    char lens = sprintf(Z_file, "\n\r create AParh.arh \n\r");
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)Z_file, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    VLCD.TextOut(Z_file,lens,8);  
    //create 890 zero page    
    for(unsigned short sec=0;sec<890;sec++){
      memset(Z_file,0,size_data_sec);
      VLCD.SetXY(10, VLCD.PosY()); VLCD.TextOut(Z_file,15,8);    
      FileWrite(pFileHndl, Z_file, size_data_sec);    
      char lens = sprintf(Z_file, "\r sector %i ",sec);
      VLCD.SetXY(10, VLCD.PosY()); VLCD.TextOut(Z_file,lens,8);
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)Z_file, lens); pCOM->StartTxD(); SimpleDelay(lens);    
      WDTreset();
    } 
    //wriet last 100 byte
    memset(Z_file,0,size_data_sec);    
    FileWrite(pFileHndl, Z_file, 100);   
    // close file
    FileClose(pFileHndl);
    //out       
    memset(Z_file,0,size_data_sec);
    lens = sprintf(Z_file, "\n\r create archive"); VLCD.TextOut(Z_file,lens,8);
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)Z_file, lens); pCOM->StartTxD(); SimpleDelay(lens);    
   delete[] Z_file; 
  }
}

void InfoBLK(CUART_BASE_ptr pCOM)
{
  if(!pCOM){      
    char need_size = 30;
    char *buf = new char[need_size];
    memset(buf,0,need_size);
    char lens = sprintf(buf, "\n\r-----------------------\n\r"); VLCD.TextOut(buf,lens,8);
    memset(buf,0,need_size);
    lens = sprintf(buf, "RAM test"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    if(!SysFlags.error_memory){lens = sprintf(buf, "- OK\n\r"); VLCD.TextOut(buf,lens,8);}
     else {lens = sprintf(buf, "- ERR\n\r"); VLCD.TextOut(buf,lens,8);}        
    memset(buf,0,need_size);
    lens = sprintf(buf, "Share RAM"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    if(!SysFlags.error_share_memory){lens = sprintf(buf, "- OK\n\r"); VLCD.TextOut(buf,lens,8);}
     else {lens = sprintf(buf, "- ERR\n\r"); VLCD.TextOut(buf,lens,8);}      
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM1"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    lens = sprintf(buf, "- %ld\n\r",COM1.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);       
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM2"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    lens = sprintf(buf, "- %ld\n\r",COM2.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);  
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM3"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    lens = sprintf(buf, "- %ld\n\r",COM3.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8); 
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM4"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    lens = sprintf(buf, "- %ld\n\r",COM4.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);       
    memset(buf,0,need_size);
    lens = sprintf(buf, "Address"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    lens = sprintf(buf, "- %i\n\r",(short)STATDTA.address); VLCD.TextOut(buf,lens,8);   
    memset(buf,0,need_size);
    lens = sprintf(buf, "-----------------------\n\r"); VLCD.TextOut(buf,lens,8); 
    delete[] buf;  
  }else{                  
    char need_size = 30;
    char *buf = new char[need_size];
    memset(buf,0,need_size);
    char lens = sprintf(buf, "\n\r-----------------------\n\r"); VLCD.TextOut(buf,lens,8);
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);
    memset(buf,0,need_size);
    
    lens = sprintf(buf, "RAM test"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    if(!SysFlags.error_memory){lens = sprintf(buf, "- OK\n\r"); VLCD.TextOut(buf,lens,8);}
     else {lens = sprintf(buf, "- ERR\n\r"); VLCD.TextOut(buf,lens,8);}        
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    

    memset(buf,0,need_size);
    lens = sprintf(buf, "Share RAM"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    if(!SysFlags.error_share_memory){lens = sprintf(buf, "- OK\n\r"); VLCD.TextOut(buf,lens,8);}
     else {lens = sprintf(buf, "- ERR\n\r"); VLCD.TextOut(buf,lens,8);}      
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);     
    
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM1"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    lens = sprintf(buf, "- %ld\n\r",COM1.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);       
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    
    memset(buf,0,need_size);    
    lens = sprintf(buf, "Port COM2"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    lens = sprintf(buf, "- %ld\n\r",COM2.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);  
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM3"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    lens = sprintf(buf, "- %ld\n\r",COM3.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    
    memset(buf,0,need_size);
    lens = sprintf(buf, "Port COM4"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    lens = sprintf(buf, "- %ld\n\r",COM4.COM_SETUP.lCOMspeed); VLCD.TextOut(buf,lens,8);       
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    
    memset(buf,0,need_size);    
    lens = sprintf(buf, "Address"); VLCD.TextOut(buf,lens,8); VLCD.SetXY(110, VLCD.PosY()); 
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);     
    lens = sprintf(buf, "- %i\n\r",(short)STATDTA.address); VLCD.TextOut(buf,lens,8);   
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    memset(buf,0,need_size);
    lens = sprintf(buf, "-----------------------\n\r"); VLCD.TextOut(buf,lens,8);       
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); SimpleDelay(lens);    
    delete[] buf; 
  }  
}

//------------------com port processing------------------------------  
void SetInt()
{
  ExtINT.INT_L = true;                   //generate interrupt for PORT R 
} 

void ResetInt()
{
  volatile char DummyInt = ExtINT.INT_R;      // reset interrupt for PORT R 
  DummyInt = ExtINT.INT_L;      // reset interrupt for PORT L 
}

// processing external interupt 
#pragma vector=INT4_vect   
__interrupt void REQ_DELAY()   
{                                  
  ResetInt();
  if(COM3.pIO_UART->cIPack_rec){
     memcpy((char*)COM3.IO_UART.aIPack, COM3.pIO_UART->aIPack, COM3.pIO_UART->sIPack_size); 
     COM3.IO_UART.sIPack_size = COM3.pIO_UART->sIPack_size; 
     COM3.IO_UART.lIPack_numb = COM3.pIO_UART->lIPack_numb;  
     COM3.pIO_UART->cIPack_rec = false;
     COM3.IO_UART.cIPack_rec = true;    
  } 
  if(COM4.pIO_UART->cIPack_rec){
     memcpy((char*)COM4.IO_UART.aIPack, COM4.pIO_UART->aIPack, COM4.pIO_UART->sIPack_size); 
     COM4.IO_UART.sIPack_size = COM4.pIO_UART->sIPack_size; 
     COM4.IO_UART.lIPack_numb = COM4.pIO_UART->lIPack_numb;  
     COM4.pIO_UART->cIPack_rec = false;
     COM4.IO_UART.cIPack_rec = true;    
  } 
  if(M_BLOCK.cNew_press){
     DATA.cKey_code = M_BLOCK.cKey_code;
     DATA.cNew_press = M_BLOCK.cNew_press;   // set flg in internal memory   
                
     TABEL_APOINT[7].value = DATA.cKey_code;
     TABEL_APOINT[8].value = DATA.cNew_press;
     
     KEYPAD.putchar(DATA.cKey_code);      
     M_BLOCK.cNew_press = false;             // clr flg in share memory   
  }   
}

//function convert MB short
unsigned short ConvertMBint(unsigned short ReadFromMB)
 {
   unsigned char tmp=0;   
   
   union{
     struct{unsigned char one; unsigned char two;} VitrualLong;     
     unsigned short  fulllong;
   }ConvertStruct;    
   ConvertStruct.fulllong = ReadFromMB;
   tmp = ConvertStruct.VitrualLong.one;
   ConvertStruct.VitrualLong.one = ConvertStruct.VitrualLong.two;
   ConvertStruct.VitrualLong.two = tmp; 
   return(ConvertStruct.fulllong);
 } 

void FillOBitField(char *buff, unsigned short pos_inout, unsigned short pos_intabl)
{ 
  //search point of input bit
  unsigned short o_byte_nmb = pos_inout/8;
  unsigned short o_pos_in_byte = pos_inout%8;  
  unsigned short t_reg_nmb = pos_intabl/16;
  unsigned short t_pos_in_reg = pos_intabl%16;  
  
  if(TABEL_OBIT[t_reg_nmb].BITFIELD & (((unsigned short)0x8000 >> t_pos_in_reg))){
    buff[o_byte_nmb] |= ((unsigned char)0x01 << (unsigned char)o_pos_in_byte);    
  }
}

void FillIBitField(char *buff, unsigned short pos_inout, unsigned short pos_intabl)
{ 
  //search point of input bit
  unsigned short o_byte_nmb = pos_inout/8;
  unsigned short o_pos_in_byte = pos_inout%8;  
  unsigned short t_reg_nmb = pos_intabl/16;
  unsigned short t_pos_in_reg = pos_intabl%16;  
  
  if(TABEL_IBIT[t_reg_nmb].BITFIELD & (((unsigned short)0x8000 >> t_pos_in_reg))){
    buff[o_byte_nmb] |= ((unsigned char)0x01 << (unsigned char)o_pos_in_byte);    
  }
}

void SetResetOutBit(unsigned short n_bit, unsigned short Command)
{ 
  if(n_bit>bit_inp_number)return;
  //search point of input bit
  unsigned short t_reg_nmb = n_bit/16;
  unsigned short t_pos_in_reg = n_bit%16;  
  // reset bit io
  if(!Command){
    TABEL_OBIT[t_reg_nmb].BITFIELD &= ~((unsigned short)0x8000 >> t_pos_in_reg);  
  }
  // set bit io
  if(Command == 0xFF00){
    TABEL_OBIT[t_reg_nmb].BITFIELD |= (unsigned short)0x8000 >> t_pos_in_reg;
  }
}
 
//--------------------work with data from port's-----------------------------
char REC_COM_BUF[sizeof(TIO_UART)/2];
   
// HOST mode port processing   
void HOSTprocessing(CUART_BASE_ptr pCOM)
{
  //COM1 processing     
  if(pCOM->IO_UART.cIPack_rec){ 
    short n_rec = pCOM->ReadIO_Buf((unsigned char*)REC_COM_BUF, sizeof(REC_COM_BUF));       
    if(((pTMREQ)REC_COM_BUF)->address==0xEB){ 
      pCOM->MODE = TERM; 
      ((pTMRESP)REC_COM_BUF)->address = 0x1D;
      SimpleDelay(4);
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, 1); pCOM->StartTxD();     
      return;         
    }
    // CRC analise
    if(CRC16_T((unsigned char*)REC_COM_BUF, n_rec)) return;
    // flash page read
    if(((pTMREQ)REC_COM_BUF)->command==0x33){                       
      FLASH.RDATA((char*)((pTRdFlash)REC_COM_BUF)->Data, size_sec,((pTRdFlash)REC_COM_BUF)->Npage, 0);           
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((((pTRdFlash)REC_COM_BUF)->Data),size_sec); 
      pCOM->StartTxD();      
    }
    // remoute pad control
    if(((pTMREQ)REC_COM_BUF)->command==0x44){                       
      //put char in key buffer
      KEYPAD.putchar(((pTMREQ)REC_COM_BUF)->start_reg_hi);          
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF,n_rec); 
      pCOM->StartTxD();      
    }    
    
    // address analise
    if(((pTMREQ)REC_COM_BUF)->address!=STATDTA.address) return;
      /*  
      RDOUT     1     // read discret output
      RDINP     2     // read discret input
      RREG      3     // read array of MB registers       
      RAINP     4     // read analog input
      WDOUT     5     // write discret output
      WREG      6     // write MB register 
      WARRREG   16    // write array of MB registers  
      */  
    unsigned short ttl_reg = (sizeof(TABEL_APOINT)+ sizeof(TABEL_DPOINT)+sizeof(TABEL_IBIT)+sizeof(TABEL_OBIT))/2;                                     
    // read discret output
    if(((pTMREQ)REC_COM_BUF)->command==RDOUT){  
      unsigned short strt_iobit = GenWfrom2B(((pTMREQ)REC_COM_BUF)->start_reg_hi, ((pTMREQ)REC_COM_BUF)->start_reg_low);
      unsigned short cnt_iobit  = GenWfrom2B(((pTMREQ)REC_COM_BUF)->numb_reg_hi, ((pTMREQ)REC_COM_BUF)->numb_reg_low);      
      //count MB register protection
      if(cnt_iobit > (bit_out_number-strt_iobit)) cnt_iobit = bit_out_number-strt_iobit; 
      memset((char*)((pTMRESP)REC_COM_BUF)->OutputBuf,0,cnt_iobit/8+1);
      for(unsigned short b_numb=0; b_numb < cnt_iobit; b_numb++){
        FillOBitField((char*)((pTMRESP)REC_COM_BUF)->OutputBuf, b_numb, strt_iobit+b_numb);
      }  
      ((pTMRESP)REC_COM_BUF)->counter = cnt_iobit/8+1;
      unsigned short CRC = CRC16_T((unsigned char*)REC_COM_BUF, cnt_iobit/8+4);      // CRC count    
      REC_COM_BUF[cnt_iobit/8+4] = LOW(CRC); REC_COM_BUF[cnt_iobit/8+5] = HIGH(CRC);     
      
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, cnt_iobit/8+6); 
      pCOM->StartTxD();               
    }
    // read discret input
    if(((pTMREQ)REC_COM_BUF)->command==RDINP){  
      unsigned short strt_iobit = GenWfrom2B(((pTMREQ)REC_COM_BUF)->start_reg_hi, ((pTMREQ)REC_COM_BUF)->start_reg_low);
      unsigned short cnt_iobit  = GenWfrom2B(((pTMREQ)REC_COM_BUF)->numb_reg_hi, ((pTMREQ)REC_COM_BUF)->numb_reg_low);      
      //count MB register protection
      if(cnt_iobit > (bit_out_number-strt_iobit)) cnt_iobit = bit_out_number-strt_iobit; 
      memset((char*)((pTMRESP)REC_COM_BUF)->OutputBuf,0,cnt_iobit/8+1);
      for(unsigned short b_numb=0; b_numb < cnt_iobit; b_numb++){
        FillIBitField((char*)((pTMRESP)REC_COM_BUF)->OutputBuf, b_numb, strt_iobit+b_numb);
      }  
      ((pTMRESP)REC_COM_BUF)->counter = cnt_iobit/8+1;
      unsigned short CRC = CRC16_T((unsigned char*)REC_COM_BUF, cnt_iobit/8+4);      // CRC count    
      REC_COM_BUF[cnt_iobit/8+4] = LOW(CRC); REC_COM_BUF[cnt_iobit/8+5] = HIGH(CRC);     
      
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, cnt_iobit/8+6); 
      pCOM->StartTxD();      
    }        
    // MB read
    if(((pTMREQ)REC_COM_BUF)->command==RREG){  
      unsigned short strt_reg = GenWfrom2B(((pTMREQ)REC_COM_BUF)->start_reg_hi, ((pTMREQ)REC_COM_BUF)->start_reg_low);
      unsigned short cnt_reg = GenWfrom2B(((pTMREQ)REC_COM_BUF)->numb_reg_hi, ((pTMREQ)REC_COM_BUF)->numb_reg_low);      
      //count MB register protection
      if(cnt_reg > (ttl_reg-strt_reg)) cnt_reg = ttl_reg-strt_reg; 
      if(cnt_reg > (sizeof(TMRESP)-3)/2) cnt_reg = (sizeof(TMRESP)-3)/2;
      for(unsigned short r_numb=0; r_numb<cnt_reg; r_numb++){
         if((r_numb+strt_reg)<ttl_reg)((pTMRESP)REC_COM_BUF)->OutputBuf[r_numb] = ConvertMBint(*(((unsigned short*)TABEL_APOINT)+r_numb+strt_reg));
          else ((pTMRESP)REC_COM_BUF)->OutputBuf[r_numb] = 0;
      }        
      ((pTMRESP)REC_COM_BUF)->counter = cnt_reg*2;
      unsigned short CRC = CRC16_T((unsigned char*)REC_COM_BUF, cnt_reg*2+3);      // CRC count    
      ((pTMRESP)REC_COM_BUF)->OutputBuf[cnt_reg] = GenWfrom2B(HIGH(CRC),LOW(CRC));  

      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, cnt_reg*2+5); 
      pCOM->StartTxD();             
    }
    // read analog input
    if(((pTMREQ)REC_COM_BUF)->command==RAINP){  
      ((pTMRESP)REC_COM_BUF)->command |= 0x80;
      ((pTMRESP)REC_COM_BUF)->counter = 0x02;
      unsigned short CRC = CRC16_T((unsigned char*)REC_COM_BUF, 3);      // CRC count  
      ((pTMRESP)REC_COM_BUF)->OutputBuf[0] = GenWfrom2B(HIGH(CRC),LOW(CRC));
      
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, 5); 
      pCOM->StartTxD();      
    }         
    // write discret output
    if(((pTMREQ)REC_COM_BUF)->command==WDOUT){  
      unsigned short strt_bit = GenWfrom2B(((pTMWRREG)REC_COM_BUF)->start_reg_hi, ((pTMWRREG)REC_COM_BUF)->start_reg_low);
      //count MB register protection
      SetResetOutBit(strt_bit, ConvertMBint(((pTMWRREG)REC_COM_BUF)->REG));         
            
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, sizeof(TMWRREG)); 
      pCOM->StartTxD();             
    }    
    // MB write
    if(((pTMREQ)REC_COM_BUF)->command==WARRREG){  
      ((pTMRESP)REC_COM_BUF)->command |= 0x80;
      ((pTMRESP)REC_COM_BUF)->counter = 0x01;
      unsigned short CRC = CRC16_T((unsigned char*)REC_COM_BUF, 3);      // CRC count  
      ((pTMRESP)REC_COM_BUF)->OutputBuf[0] = GenWfrom2B(HIGH(CRC),LOW(CRC));  
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, 5); 
      pCOM->StartTxD();         
    }   
    if(((pTMREQ)REC_COM_BUF)->command==WREG){        
      unsigned short strt_reg = GenWfrom2B(((pTMWRREG)REC_COM_BUF)->start_reg_hi, ((pTMWRREG)REC_COM_BUF)->start_reg_low);
      //count MB register protection
      if(strt_reg > ttl_reg) return; 
      *(((unsigned short*)TABEL_APOINT) + strt_reg) = ConvertMBint(((pTMWRREG)REC_COM_BUF)->REG);         
            
      pCOM->StopTxD();
      pCOM->LoadIO_Buf((unsigned char*)REC_COM_BUF, sizeof(TMWRREG)); 
      pCOM->StartTxD();  
      return;                 
    }      
  } // if(pCOM->IO_UART.cIPack_rec)..
}

// TERM mode port processing   
void TERMprocessing(CUART_BASE_ptr pCOM)
{  
  char t_buf[3]; char v_buf[6];
  char lens = 0;
  //COM1 processing     
  if(pCOM->IO_UART.cIPack_rec){ 
    if(SMODE==RECFILE){
      FileDataProc(pCOM);              
      return;
    }
    char n_rec = pCOM->ReadIO_Buf((unsigned char*)t_buf, sizeof(t_buf));
    if(SMODE!=TERMINAL){
       //connect analise
       if(n_rec){
         SMODE = TERMINAL;
         VLCD.Clr(); SimpleDelay(50);  
         lens = sprintf(v_buf, "\nC:>"); VLCD.TextOut(v_buf,lens,8);
         pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)v_buf, lens); pCOM->StartTxD();           
         }else {pCOM->MODE = HOST;}
       return;
    } 
    // command analise 
    if(SMODE==TERMINAL){
      switch (t_buf[0]){
       case 0x02 : {break;}
       case 0x0D : { 
                     StrProcessor(pCOM);                     
                     KEYPAD.clrkeybuf();
                     lens = sprintf(v_buf, "\r\nC:>"); VLCD.TextOut(v_buf,lens,8); 
                     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)v_buf, lens); pCOM->StartTxD();  
                     break;
                   }      
       case 0x1B : {  
                     KEYPAD.clrkeybuf();
                     lens = sprintf(v_buf, "\r\nC:>"); VLCD.TextOut(v_buf,lens,8); 
                     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)v_buf, lens); pCOM->StartTxD();    
                     break;
                   }  
       case 0x08 : { break;} 
       case 0x3F : { 
                     KEYPAD.clrkeybuf();                     
                     lens = sprintf(v_buf, "!"); 
                     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)v_buf, lens); pCOM->StartTxD();    
                     SMODE = RECFILE;  
                     break;                   
                   }  
       default   : {
                     KEYPAD.putchar(t_buf[0]);   // put in keypad buffer                         
                     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)t_buf, 1); pCOM->StartTxD();   //return code                     
                     VLCD.TextOut(t_buf,1,8);               
                     break;
                   }       
      } // switch (t_buf[0])..     
    } // if(SMODE==TERMINAL)..                        
  } 
}

// command string processing
void StrProcessor(CUART_BASE_ptr pCOM)
{
  unsigned short Nport=0,Parity=0,N_stbit=0; 
  unsigned short Hour=0,Min=0,Sec=0,mSec=0;  
  unsigned long ComSpeed=0;
  char need_size = 30; char *buf = new char[need_size]; memset(buf,0,need_size);
  need_size = 13; char *name_buf = new char[need_size]; memset(name_buf,0,need_size);
    sscanf(KEYPAD.KeyBuf.KeypadBuf,"%s %d %d %d %d",buf,&Hour,&Min,&Sec,&mSec);
    sscanf(KEYPAD.KeyBuf.KeypadBuf,"%s %d %ld %c %d",buf,&Nport,&ComSpeed,&Parity,&N_stbit);    
    sscanf(KEYPAD.KeyBuf.KeypadBuf,"%s %s",buf,name_buf);
    
    switch (Gethash(buf)){
      case 763 : {SetTime((unsigned short)Hour,Min,Sec);return;}                        // settime hh:mm:ss 
      case 742 : {SetData((unsigned short)Hour,Min,Sec);return;}                        // setdata yy:mm:dd
      case 785 : {SetPort((char)Nport,ComSpeed,(char)Parity,(char)N_stbit);return;}     // setport 1 9600 E 1
      case 743 : {STATDTA.address = (char)Hour;return;}                                 // setaddr 1  
      case 627 : {DeleteFile(name_buf);return;}                                         // delete <filename.ext>
    }   
  
    switch (KEYPAD.gethash()){
      case 779 : {InfoBLK(pCOM); break;}                                 // sysinfo
      case 319 : {DIR(pCOM);break;}                                      // dir
      case 533 : {InitArhiv(pCOM);return;}                               // ainit     
      case 649 : {FormatFlash(pCOM);break;}                              // format
      case 773 : {while(true){MASTER_BLOCK.cMasterInit = 0xEA;}}    // restart & reinit objects
      default   : {                  
                    char lens = sprintf(buf, "\n\rbad command"); VLCD.TextOut(buf,lens,8);
                    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();   
                    break;   
                  }       
    }
  delete[] buf;     
  delete[] name_buf; 
}

void PortProcessor()
{    
  switch (COM1.MODE){
    case HOST : {if(SMODE==NORMAL)HOSTprocessing(&COM1); break;}      
    case TERM : {TERMprocessing(&COM1); break;}  
    default   : {break;}       
  }     
  switch (COM2.MODE){
    case HOST : {if(SMODE==NORMAL)HOSTprocessing(&COM2); break;}      
    case TERM : {TERMprocessing(&COM2); break;}  
    default   : {break;}       
  }    
  switch (COM3.MODE){
    case HOST : {if(SMODE==NORMAL)HOSTprocessing(&COM3); break;}      
    case TERM : {TERMprocessing(&COM3); break;}  
    default   : {break;}       
  }    
  switch (COM4.MODE){
    case HOST : {if(SMODE==NORMAL)HOSTprocessing(&COM4); break;}      
    case TERM : {TERMprocessing(&COM4); break;}  
    default   : {break;}       
  }             
} 

//-------------------------Alternative file system---------------------------------------
//function return first free sector in file sistem
unsigned short GetFreeSec()
{
  #define tmp_b_size 264
  unsigned short find_sec = 0;
  unsigned short page_numb = fat_page;
  //unsigned short byte_numb = 0;

  char *tmpB = new char[tmp_b_size];
  while(page_numb < root_page){
      pFLASH->RDATA(tmpB,tmp_b_size,page_numb,0);
      // search free bit in tmmB
      for(unsigned short numb=0;numb<tmp_b_size;numb++){
        char cur_byte = *(tmpB+numb);
        if(cur_byte != 0xFF){              
          for(unsigned char  bite_numb=0;bite_numb<8;bite_numb++){
            if(!((0x80>>bite_numb) & cur_byte)){
              find_sec = (page_numb-fat_page)*size_sec*8 + numb*8 + bite_numb;
              delete[] tmpB; 
              MarkFreeSec(find_sec + dstrt_page);
              return(find_sec + dstrt_page);
            }
          }
        }
      } // for(char numb=0;numb<tmp_b_size;numb++)
    page_numb++;
  }      
  delete[] tmpB; 
  return(0);
}

//function delete busy sector in fat
bool DelBusySec(unsigned short Sector)
{
  //delite sector in data area and next sector pointer(nsp)
  pFLASH->CLRsec(Sector); 
   
  Sector-=dstrt_page;
  unsigned short page_numb = fat_page + Sector/(size_sec*8);
  unsigned short byte_numb = (Sector%(size_sec*8))/8;  
  unsigned short bit_numb  = (Sector%(size_sec*8))%8;
  char byte_sec = 0;
  //red byte associated with sector
  if(!pFLASH->RDATA(&byte_sec,1,page_numb,byte_numb))return(false);
  //modyfi byte 
  byte_sec &=~(0x80>>bit_numb);
  //write byte 
  if(!pFLASH->WDATA(&byte_sec,1,page_numb,byte_numb))return(false);
  return(true);
}

//function occupy free sector in fat
bool MarkFreeSec(unsigned short Sector)
{
  //delite sector in data area and next sector pointer(nsp)
  pFLASH->CLRsec(Sector); 
  
  Sector-=dstrt_page;  
  unsigned short page_numb = fat_page + Sector/(size_sec*8);
  unsigned short byte_numb = (Sector%(size_sec*8))/8;  
  unsigned short bit_numb  = (Sector%(size_sec*8))%8;
  char byte_sec = 0;
  //red byte associated with sector
  if(!pFLASH->RDATA(&byte_sec,1,page_numb,byte_numb))return(false);
  //modyfi byte 
  byte_sec |=(0x80>>bit_numb);
  //write byte 
  if(!pFLASH->WDATA(&byte_sec,1,page_numb,byte_numb))return(false);  
  return(true);
}

//write data in file
unsigned short FileWrite(pTFileHndl pFileH, char*buff, unsigned short size){ 
  unsigned short need_sector = pFileH->FRec.StrtSector;       // start file sector
  unsigned short sector = pFileH->Fpoiner/size_data_sec;      // number of the sector in file
  unsigned short offset = pFileH->Fpoiner%size_data_sec;      // offset in file
  unsigned short wrt_size = 0;
  unsigned short new_sector=0; 
  if(!pFileH) return(0);  
  //search need sector
  for(unsigned short att=0;att<sector;att++){
    if(!pFLASH->RDATA((char*)&need_sector,sizeof(need_sector),need_sector,size_data_sec))return(0);  
  }
  while(size>wrt_size){
    if((size-wrt_size)>(size_data_sec-offset)){
     pFLASH->WDATA(buff+wrt_size,size_data_sec-offset,need_sector,offset);
     wrt_size += size_data_sec-offset;          
     pFileH->Fpoiner+=size_data_sec-offset;          
     offset = 0;      
     // read next sector     
     pFLASH->RDATA((char*)&new_sector,sizeof(new_sector),need_sector,size_data_sec);
     if(!new_sector){                
       new_sector=GetFreeSec();
       if(new_sector){
         pFLASH->WDATA((char*)&new_sector,sizeof(new_sector),need_sector,size_data_sec);  
         need_sector = new_sector;         
       }else{          
         //alarm exit
         if(pFileH->Fpoiner > pFileH->FRec.FileSize){
           pFileH->FRec.FileSize += (pFileH->Fpoiner - pFileH->FRec.FileSize);     //set size
         }          
        return(wrt_size);              
       } 
     }else{need_sector = new_sector;}  
    }else{         
       //write last block
       pFLASH->WDATA(buff+wrt_size,size-wrt_size,need_sector,offset);
       //set pointer and size       
       pFileH->Fpoiner+=(size-wrt_size); 
       //next sector reservation    
       pFLASH->RDATA((char*)&new_sector,sizeof(new_sector),need_sector,size_data_sec);
       if(!new_sector){        
         if(!(pFileH->Fpoiner%size_data_sec)){ 
           new_sector=GetFreeSec();
           pFLASH->WDATA((char*)&new_sector,sizeof(new_sector),need_sector,size_data_sec);        
         }              
       }       
       if(pFileH->Fpoiner > pFileH->FRec.FileSize){
         pFileH->FRec.FileSize += (pFileH->Fpoiner - pFileH->FRec.FileSize);     //set size
       }  
       wrt_size = size; 
    }  
  } // while(size) 
  return(size);
}

//read data from file
unsigned short FileRead(pTFileHndl pFileH, char*buff, unsigned short size)
{
  unsigned short red_size = size; 
  unsigned short need_sector = pFileH->FRec.StrtSector;
  unsigned short sector = pFileH->Fpoiner/size_data_sec;
  unsigned short offset = pFileH->Fpoiner%size_data_sec;  
  unsigned short rd_size = 0; 
  //protection
  if(!pFileH) return(0);  
  if(pFileH->Fpoiner+size>pFileH->FRec.FileSize){
    red_size = pFileH->FRec.FileSize-pFileH->Fpoiner;
  }  
  //search need sector
  for(unsigned short att=0;att<sector;att++){
    if(!pFLASH->RDATA((char*)&need_sector,sizeof(need_sector),need_sector,size_data_sec))return(0);  
  }
  while(red_size>rd_size){
    if((red_size - rd_size)>(size_data_sec-offset)){    
     pFLASH->RDATA(buff+rd_size,size_data_sec-offset,need_sector,offset);
     
     rd_size += size_data_sec-offset;
     pFileH->Fpoiner+=size_data_sec-offset;
     offset = 0;
     unsigned short new_sector=0;
     pFLASH->RDATA((char*)&new_sector,sizeof(new_sector),need_sector,size_data_sec);
     if(new_sector) need_sector = new_sector;
      else return(rd_size);   
    }else{
       pFLASH->RDATA(buff+rd_size,red_size - rd_size,need_sector,offset);
       pFileH->Fpoiner+=red_size - rd_size; 
       rd_size = red_size;
    }  
  } // while(size)
 
  return(red_size);
}  

//Repositions read/write point. 
long FileSeek(pTFileHndl pFileH, unsigned long offset, char Origin)
{
  switch (Origin){
    case SEEK_SET : {if(offset>pFileH->FRec.FileSize)pFileH->Fpoiner = pFileH->FRec.FileSize;
                      else pFileH->Fpoiner = offset; 
                     break;
                    }      
    case SEEK_CUR : {if((pFileH->Fpoiner + offset)>pFileH->FRec.FileSize)pFileH->Fpoiner = pFileH->FRec.FileSize; 
                      else pFileH->Fpoiner += offset; 
                     break;
                    }   
    case SEEK_END : {if(pFileH->FRec.FileSize>offset)pFileH->Fpoiner = pFileH->FRec.FileSize-offset; 
                      else pFileH->Fpoiner = 0; 
                      break;
                    }    
    default  : { return(-1);}       
  }    
  return(pFileH->Fpoiner);
}

// create file in flash
pTFileHndl CreateFile(char *PrjFile)
{
  //tst MBR&ROOT signature  
  pTSBLK MBR = new TSBLK;
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR;return(NULL);}
  delete MBR;
  return(AddToFileListNew(PrjFile));
}

// close file
void FileClose(pTFileHndl pFileH)
{
  if(FilesList.FileObjCntr>0){
    if(pFileH->mode == f_new){      
     pFileH->mode = 0;      
     //tst MBR&ROOT signature / modification LastFreeSector 
     pTSBLK MBR = new TSBLK;     
       pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
       if(MBR->Signature!=mFAT_OK){delete MBR; return;}                         
       MBR->BusySector +=(pFileH->FRec.FileSize)/size_sec+1;    
       MBR->CRC = CRC16_T((unsigned char*)(&MBR), sizeof(TSBLK)-2);
       pFLASH->WDATA((char*)MBR, sizeof(TSBLK),boot_page,0);       
     delete MBR; 
     //read & modification ROOT
     pTROOTCAT ROOT = new TROOTCAT;
       pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
       if(ROOT->Signature != 0xAAAA){delete ROOT; return;}       
       unsigned short CurentRecord = ROOT->TotalRecord++; 
       ROOT->BlkCRC = CRC16_T((unsigned char*)(&ROOT), sizeof(TROOTCAT)-2);   
       pFLASH->WDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);      
     delete ROOT;    
     // save record
     TFilePoint FilePoint = FileExist((char*)pFileH->FRec.FileName);
     if(!(FilePoint.sector || FilePoint.offset)){       
       FilePoint = GetFreeROOTPos(); 
     }else{DeleteFile((char*)pFileH->FRec.FileName);}
     pFLASH->WDATA((char*)&pFileH->FRec, sizeof(TFileRecord),FilePoint.sector,FilePoint.offset);     
    } // if(pFileH->mode == f_new)
    if(pFileH->mode == f_wr){
      // save record
      TFilePoint FilePoint = FileExist((char*)pFileH->FRec.FileName);  
      if(FilePoint.sector || FilePoint.offset){     
        pFLASH->WDATA((char*)&pFileH->FRec, sizeof(TFileRecord),FilePoint.sector,FilePoint.offset);   
      }  
    }   
    DelFromFileList(pFileH);   
  }  
  pFileH = NULL;
}

// open file
pTFileHndl OpenFile(char *PrjFile, char mode)
{
  pTFileHndl pFileHndl = NULL; 
  //tst MBR&ROOT signature 
  pTSBLK MBR = new TSBLK;
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR;return(NULL);} 
  delete MBR; 
  //read ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
    if(ROOT->Signature != 0xAAAA){delete ROOT; return(NULL);} 
    char rec_nmb = ROOT->TotalRecord;
  delete ROOT;
  
  pTFileRecord FREC = new TFileRecord;    
  unsigned short n_rec=0;
  for(unsigned short n_real_rec=0;n_real_rec<rec_nmb;n_real_rec++,n_rec++){     
    unsigned short sector = (sizeof(TFileRecord)*n_rec + sizeof(TROOTCAT))/size_sec+root_page;
    unsigned short offset = sizeof(TFileRecord)*n_rec%size_sec + sizeof(TROOTCAT);
    pFLASH->RDATA((char*)FREC, sizeof(TFileRecord),sector,offset);
    if(FREC->FileName[0]!= 0){
      if(strcmp(PrjFile,(char*)FREC->FileName)) continue; 
       else{
         pFileHndl = AddToFileListExist(PrjFile);
         pFileHndl->Fpoiner = 0;
         pFileHndl->mode = mode;
         memcpy((char*)&(pFileHndl->FRec),(char*)FREC, sizeof(TFileRecord));
         break;
       } 
    }else{n_real_rec--;}               
  }  
  delete FREC;    
  return(pFileHndl);
}

// check exist file  
TFilePoint FileExist(char *PrjFile)
{
  TFilePoint exists; 
  exists.sector = 0; exists.offset = 0;
  //tst MBR&ROOT signature 
  pTSBLK MBR = new TSBLK;
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR;return(exists);} 
  delete MBR; 
  //read ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
    if(ROOT->Signature != 0xAAAA){delete ROOT; return(exists);} 
    char rec_nmb = ROOT->TotalRecord;
  delete ROOT;
  pTFileRecord FREC = new TFileRecord;
  unsigned short n_rec=0;
  for(unsigned short n_real_rec=0;n_real_rec<rec_nmb;n_real_rec++,n_rec++){     
    unsigned short sector = (sizeof(TFileRecord)*n_rec + sizeof(TROOTCAT))/size_sec+root_page;
    unsigned short offset = sizeof(TFileRecord)*n_rec%size_sec + sizeof(TROOTCAT);
    pFLASH->RDATA((char*)FREC, sizeof(TFileRecord),sector,offset);
    if(FREC->FileName[0]!= 0){
      if(strcmp(PrjFile,(char*)FREC->FileName)) continue; 
       else{exists.sector = sector; exists.offset = offset; break;} 
    }else{n_real_rec--;}               
  }
  delete FREC;    
  return(exists);
}

bool DeleteFile(char *PrjFile)
{     
  //tst MBR&ROOT signature / modification LastFreeSector 
  pTSBLK MBR = new TSBLK;     
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR; return(false);}                              
  delete MBR;   
  //read & modification ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
    if(ROOT->Signature != 0xAAAA){delete ROOT; return(false);}          
  delete ROOT;   

   // save record
   unsigned short StrtSector=0;     
   unsigned long  FileSize=0;
   TFilePoint FilePoint = FileExist(PrjFile);
   if(FilePoint.sector || FilePoint.offset){       
     //delete record in ROOT  
     pTFileRecord pFREC = new TFileRecord;
       pFLASH->RDATA((char*)pFREC, sizeof(TFileRecord),FilePoint.sector,FilePoint.offset);
       StrtSector = pFREC->StrtSector; FileSize = pFREC->FileSize;
       memset((char*)pFREC,0, sizeof(TFileRecord));
       pFLASH->WDATA((char*)pFREC, sizeof(TFileRecord),FilePoint.sector,FilePoint.offset); 
     delete pFREC;      
     //delete file from flash
     while(StrtSector){
       unsigned short DelSector=StrtSector; 
       pFLASH->RDATA((char*)&StrtSector,sizeof(StrtSector),StrtSector,size_data_sec);
       DelBusySec(DelSector);                 
     }  

     //tst MBR&ROOT signature / modification LastFreeSector 
     pTSBLK MBR = new TSBLK;     
       pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
       if(MBR->Signature!=mFAT_OK){delete MBR; return(true);}                         
       MBR->BusySector -=(FileSize)/size_sec;     
       MBR->CRC = CRC16_T((unsigned char*)(&MBR), sizeof(TSBLK)-2);
       pFLASH->WDATA((char*)MBR, sizeof(TSBLK),boot_page,0);       
     delete MBR;   
     //read & modification ROOT
     pTROOTCAT ROOT = new TROOTCAT;
       pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
       if(ROOT->Signature != 0xAAAA){delete ROOT; return(true);}       
       unsigned short CurentRecord = ROOT->TotalRecord--; 
       ROOT->BlkCRC = CRC16_T((unsigned char*)(&ROOT), sizeof(TROOTCAT)-2);   
       pFLASH->WDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);      
     delete ROOT;    
     return(true);              
   } // end if(FilePoint.sector || FilePoint.offset)...
   
  return(false);   
}

TFilePoint GetFreeROOTPos()
{
  TFilePoint exists; 
  exists.sector = 0; exists.offset = 0;
  //tst MBR&ROOT signature 
  pTSBLK MBR = new TSBLK;
    pFLASH->RDATA((char*)MBR, sizeof(TSBLK),boot_page,0);     
    if(MBR->Signature!=mFAT_OK){delete MBR; return(exists);} 
  delete MBR; 
  //read ROOT
  pTROOTCAT ROOT = new TROOTCAT;
    pFLASH->RDATA((char*)ROOT, sizeof(TROOTCAT),root_page,root_strt);
    if(ROOT->Signature != 0xAAAA){delete ROOT; return(exists);} 
  delete ROOT;
  pTFileRecord FREC = new TFileRecord;
  for(unsigned short n_real_rec=0;n_real_rec<160;n_real_rec++){     
    unsigned short sector = (sizeof(TFileRecord)*n_real_rec + sizeof(TROOTCAT))/size_sec+root_page;
    unsigned short offset = sizeof(TFileRecord)*n_real_rec%size_sec + sizeof(TROOTCAT);
    pFLASH->RDATA((char*)FREC, sizeof(TFileRecord),sector,offset);
    if(FREC->FileName[0]== 0){exists.sector = sector; exists.offset = offset; break;}               
  }
  delete FREC;    
  return(exists);
}

//create file header in list and his inicialisation 
pTFileHndl AddToFileListNew(char *PrjFile)
{
  pTFileListObj pFileListObj = new TFileListObj;          // pointer on next file object
   
  if(pFileListObj){
    pFileListObj->NextFileObj = NULL;
    if(FilesList.FileObjCntr == 0){FilesList.pStrtFileObj =  FilesList.pLstFileObj = pFileListObj;}
    else{
      FilesList.pLstFileObj->NextFileObj = pFileListObj; FilesList.pLstFileObj = pFileListObj;
    }   
    unsigned short LFSector = GetFreeSec();     //search last sector
    if(!LFSector)return(NULL);         
    // store file info       
    pFileListObj->FileHndl.Fpoiner = 0;     
    pFileListObj->FileHndl.mode = f_new;    
    memset((char*)&(pFileListObj->FileHndl.FRec),0, sizeof(TFileRecord));
    memcpy((char*)&(pFileListObj->FileHndl.FRec.FileName),PrjFile, 12);
    pFileListObj->FileHndl.FRec.StrtSector = LFSector;    
       
    FilesList.FileObjCntr++;   
  }else{return(NULL);}   
  return(&(pFileListObj->FileHndl));
}  

//create file header in list 
pTFileHndl AddToFileListExist(char *PrjFile)
{
  pTFileListObj pFileListObj = new TFileListObj;          // pointer on next file object
   
  if(pFileListObj){
    pFileListObj->NextFileObj = NULL;
    if(FilesList.FileObjCntr == 0){FilesList.pStrtFileObj =  FilesList.pLstFileObj = pFileListObj;}
    else{
      FilesList.pLstFileObj->NextFileObj = pFileListObj; FilesList.pLstFileObj = pFileListObj;
    }           
    // store file info       
    memset((char*)&(pFileListObj->FileHndl.FRec),0, sizeof(TFileRecord));
    memcpy((char*)&(pFileListObj->FileHndl.FRec.FileName),PrjFile, 12);          
    FilesList.FileObjCntr++;   
  }else{return(NULL);}   
  return(&(pFileListObj->FileHndl));
}

//delete file header in list and his inicialisation 
bool DelFromFileList(pTFileHndl pFileHndl)
{
  if(FilesList.FileObjCntr == 0)return(false);  
  if(!pFileHndl)return(false); 
  pTFileListObj pFObj = FilesList.pStrtFileObj;  
  //if items - first element 
  if(&(pFObj->FileHndl) == pFileHndl){
    FilesList.pStrtFileObj = pFObj->NextFileObj;
    //delite items  
    delete pFObj;
    FilesList.FileObjCntr--;
    return(true);      
  }
  //find prevent items
  while(&(pFObj->NextFileObj->FileHndl) != pFileHndl){
   if(!pFObj->NextFileObj)return(false);
   pFObj = pFObj->NextFileObj;     
  } 
  //reinit tail pointer 
  pTFileListObj pDelObj = pFObj->NextFileObj;
  if(pFObj->NextFileObj->NextFileObj){pFObj->NextFileObj = pFObj->NextFileObj->NextFileObj;}
    else{FilesList.pLstFileObj = pFObj; pFObj->NextFileObj = NULL;}
   //delite items  
  delete pDelObj;
  FilesList.FileObjCntr--;                            
 
  return(true);
}    
  
#define BLOCKTYPE_HEADER 1
#define BLOCKTYPE_DATA   2
#define SIGNATURE        29

#define WAIT_BLK_HEAD  1 // Mmode
#define WAIT_BLK_DATA  2
#define WAIT_FILE_HEAD 1 // Smode 
#define WAIT_FILE_DATA 2

void FileDataProc(CUART_BASE_ptr pCOM)
{
  static char Mmode = WAIT_FILE_HEAD;
  static char Smode = WAIT_BLK_HEAD;  
  static pTFileHndl NewFile;
  static unsigned short BLK_N=0;           // wait block number
  char buf[10]; unsigned short n_rec; char lens;
           
  if(Mmode==WAIT_FILE_HEAD){
    // wait recieve file header  
    if(Smode==WAIT_BLK_HEAD){
      n_rec = pCOM->ReadIO_Buf((unsigned char*)REC_COM_BUF, sizeof(REC_COM_BUF)); 
      pTBlock pBlock = (pTBlock)REC_COM_BUF;
      //type & signature analise
      if((pBlock->Type != BLOCKTYPE_HEADER) || (pBlock->Sign != SIGNATURE)){
        // error ACK
        lens = sprintf(buf, "-"); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();        
        return;
      }      
      lens = sprintf(buf, "load "); VLCD.TextOut(buf,lens,8); 
      // need data block ACK 
      lens = sprintf(buf, ">"); 
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();          
      Smode=WAIT_BLK_DATA;    //  next package - data
    }else{
      //create file heared and him wrait in file
      n_rec = pCOM->ReadIO_Buf(((unsigned char*)REC_COM_BUF)+sizeof(TBlock), sizeof(REC_COM_BUF)-sizeof(TBlock)); 
      pTFileHeader pFileHeader = (pTFileHeader)((unsigned char*)REC_COM_BUF+sizeof(TBlock));
      //crc analise
      if(CRC16_T((unsigned char*)REC_COM_BUF, n_rec+sizeof(TBlock))){
        // CRC error ACK
        lens = sprintf(buf, "e"); VLCD.TextOut(buf,lens,8); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf,lens); pCOM->StartTxD();        
        return;
      }     
      //create file
      NewFile = CreateFile(pFileHeader->FileName); 
      VLCD.TextOut(pFileHeader->FileName,12,8);          
      // file record fill 
      if(NewFile){     
        lens = sprintf(buf, "["); VLCD.TextOut(buf,lens,8);  
        NewFile->FRec.RecTime  = pFileHeader->FileTime;
        NewFile->FRec.RecData  = pFileHeader->FileDate;
        //NewFile->FRec.FileSize = pFileHeader->FileSize;
        NewFile->FRec.FileCRC  = pFileHeader->FileCRC;        
      }     
      // OK & need next block ACK
      lens = sprintf(buf, "+."); 
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();                 
      Smode=WAIT_BLK_HEAD;    //  next package - heder      
      Mmode=WAIT_FILE_DATA;
    }
  }else{    
    // wait recieve file data  
    if(Smode==WAIT_BLK_HEAD){ 
      n_rec = pCOM->ReadIO_Buf((unsigned char*)REC_COM_BUF, sizeof(REC_COM_BUF)); 
      pTBlock pBlock = (pTBlock)REC_COM_BUF;
      //type & signature analise
      if((pBlock->Type != BLOCKTYPE_DATA) || (pBlock->Sign != SIGNATURE)){
        // error ACK
        lens = sprintf(buf, "-"); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();        
        return;
      }  
      // need data block ACK        
      lens = sprintf(buf, ">");VLCD.TextOut(buf,lens,8);  
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();        
      Smode=WAIT_BLK_DATA;    //  next package - data
    }else{
      n_rec = pCOM->ReadIO_Buf(((unsigned char*)REC_COM_BUF)+sizeof(TBlock), sizeof(REC_COM_BUF)-sizeof(TBlock));             
      pTFileData pFileData = (pTFileData)((unsigned char*)REC_COM_BUF+sizeof(TBlock));
      //crc analise
      if(CRC16_T((unsigned char*)REC_COM_BUF, n_rec+sizeof(TBlock))){
        // crc error ACK
        lens = sprintf(buf, "e"); VLCD.TextOut(buf,lens,8); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf,lens); pCOM->StartTxD();        
        return;
      }
      // necessary block analise
      if(BLK_N!=pFileData->BlockN){
        // error ACK
        lens = sprintf(buf, "-"); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD(); 
        return;    
      }
      //write in file      
      if(NewFile){FileWrite(NewFile, pFileData->Data, n_rec-6); SimpleDelay(10);}         
      // OK ACK 
      lens = sprintf(buf, "+"); 
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();               
      //last block analise
      if(pFileData->BlockN >= (pFileData->BlockCount-1)){
        FileClose(NewFile);                                       // close load file                
        lens = sprintf(buf, "]"); VLCD.TextOut(buf,lens,8);  
        //last block ACK
        lens = sprintf(buf, "+ OK."); VLCD.TextOut(buf,lens,8);  
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();        
        SimpleDelay(20);
        lens = sprintf(buf, "\r\nC:>"); VLCD.TextOut(buf,lens,8); 
        pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();  
        //return in start position
        Mmode = WAIT_FILE_HEAD; Smode = WAIT_BLK_HEAD;         
        BLK_N=0; NewFile = NULL; SMODE = TERMINAL;    
        return;                
      }            
      // need next block ACK
      BLK_N++;  
      SimpleDelay(2);
      lens = sprintf(buf, "."); 
      pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)buf, lens); pCOM->StartTxD();        
      Smode=WAIT_BLK_HEAD;    //  next package - heder
    } // if(Smode==WAIT_BLK_HEAD)..else..   
  }// if(Mmode==WAIT_FILE_HEAD)..else..

}
//---------------------------Visual_X processing------------------------------

C_WIN_prt CreateWin(ptrTVisualObj WindowDTA)
{    
  C_WIN_prt Win = new C_WIN(WindowDTA,W_COMMON);       
  Win->NextWin   = NULL;

  if(!WinList.WindCounter){
    WinList.FirstWind =  WinList.LastWind = Win;
    WinList.LastWind->Active = true;
  }
  else{
    WinList.LastWind->NextWin = Win;
    WinList.LastWind->Active = false;      
    WinList.LastWind = Win;
    WinList.LastWind->Active = true;
  } 
  WinList.LastWind->win_nmbr = ++WinList.WindCounter;
  return(Win);      
}    

void DestroyWin()
{
  if(WinList.WindCounter>1){
    C_WIN_prt Win = WinList.FirstWind; 
    //destroy window search 
    while(Win->NextWin->NextWin){Win = Win->NextWin;}  
    C_WIN_prt DelWin = Win->NextWin;

    Win->NextWin = NULL;
    WinList.LastWind = Win;
    WinList.WindCounter--;    
    DelWin->Destroy();
  }  
}                                                                         

void WinObjProcessor()
{   
  if(WinList.WindCounter){
    //fill a&d point object
    TObject_ptr pVObj = WinList.LastWind->ObjList.FirstObj;
    while(pVObj){    
      switch (pVObj->ObjID){
        // fill A point
        case APOINT : {break;}      
        case DPOINT : {break;}      // fill D point
        default  : {break;}       
      }     
     pVObj = pVObj->NextObj;
    }          
  } 
}

void WinRedrawProcessor()
{  
  if(SMODE!=NORMAL) return;
  if(FLGS.DataLatch==BUSY){
    if(WinList.WindCounter){ 
      WinObjProcessor();
      WinList.LastWind->Draw();     
    }  
    EndUpdate();
  }    
}

//--------------------open project file-----------------------------
union{
  TWorkFileSP WorkFileSP; TContBut ContBut;
  TContAP ContAP;         TContDP ContDP;
  TContLabel ContL;       TContBitMap ContBM;
  TContAE ContAE; 
}CommonW;

bool OpenPjgFile(char *PrjFile)
{  
  char nmb_but,nmb_ap,nmb_ae,nmb_dp,nmb_label,nmb_bitmap;  
  if(WinList.WindCounter>15)return(false);
  pTFileHndl iFileHandle = OpenFile(PrjFile,f_rd); 
  if(!iFileHandle){
    char *err_b = new char[25];memset(err_b,0,sizeof(err_b));
    char lens = sprintf(err_b, "%s not fond",PrjFile);  
    OutMsg(err_b, lens,0);            
    delete[] err_b;   
    return(false);
  }
  FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.WorkFileSP));

  nmb_but = CommonW.WorkFileSP.nmb_but;
  nmb_ap  = CommonW.WorkFileSP.nmb_ap;
  nmb_dp  = CommonW.WorkFileSP.nmb_dp; 
  nmb_label  = CommonW.WorkFileSP.nmb_label; 
  nmb_bitmap = CommonW.WorkFileSP.nmb_bitmap; 
  nmb_ae = CommonW.WorkFileSP.nmb_ae;
   
  C_WIN_prt Win = CreateWin(&(CommonW.WorkFileSP.WinData.ObjDTA));
  if(Win){
    for(char i=0;i<nmb_but;i++){
      //CrButton(ptrTVisualObj ButDTA, char butKind, void* pPoint, char *FileName, char SetVal);
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContBut));      
      void* pPoint = NULL;
      if(CommonW.ContBut.Kind==bkChApoint) pPoint = (TABEL_APOINT+CommonW.ContBut.NumbPoint);
      if(CommonW.ContBut.Kind==bkChDpoint) pPoint = (TABEL_DPOINT+CommonW.ContBut.NumbPoint);
      Win->CrButton(&CommonW.ContBut.ObjDTA,CommonW.ContBut.Kind,pPoint
                                         ,CommonW.ContBut.FileName,CommonW.ContBut.SetValue); 
    }
    for(char i=0;i<nmb_ap;i++){
      //CrAparam(ptrTVisualObj ObjData, pTAPOINT pPointR, pTAPOINT pPointW, bool frame_type, bool modifiable, char PLC_node)
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContAP));      
      pTAPOINT pPointR = (TABEL_APOINT+CommonW.ContAP.NumbPointR);
      pTAPOINT pPointW = (TABEL_APOINT+CommonW.ContAP.NumbPointW);
      Win->CrAparam(&CommonW.ContAP.ObjDTA,pPointR,pPointW,CommonW.ContAP.frame_type
                                         ,CommonW.ContAP.modifiable,CommonW.ContAP.PLC_node);     
    }
    for(char i=0;i<nmb_dp;i++){
      //CrDparam(ptrTVisualObj ObjData, pTDPOINT pPoint, char *LoStr, char *HiStr, char PLC_node);
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContDP)); 
      pTDPOINT pPoint = (TABEL_DPOINT+CommonW.ContDP.NumbPoint);
      Win->CrDparam(&CommonW.ContDP.ObjDTA,pPoint,CommonW.ContDP.LowString
                                         ,CommonW.ContDP.HiString,CommonW.ContDP.PLC_node);         
    }
    for(char i=0;i<nmb_label;i++){
      //CrLabel(ptrTVisualObj ButDTA);
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContL)); 
      Win->CrLabel(&CommonW.ContL.ObjDTA);
    }          
    for(char i=0;i<nmb_bitmap;i++){
      //CrBitMap(ptrTVisualObj ObjData, char*WinFile,char*BitMapFile, 
                  //bool modifiable, pTDPOINT InitPoint, char init_value, unsigned short move_time)
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContBM)); 
      pTDPOINT pPoint = (TABEL_DPOINT+CommonW.ContBM.NumbPoint);
      Win->CrBitMap(&CommonW.ContBM.ObjDTA,CommonW.ContBM.FileName,CommonW.ContBM.BMFileName
                 ,CommonW.ContBM.modifiable,pPoint,CommonW.ContBM.InitValue,CommonW.ContBM.TimeRdrw);  
    }
    for(char i=0;i<nmb_ae;i++){
      //CrAedit(ptrTVisualObj ObjData, bool frame_type)
      FileRead(iFileHandle, (char*)&CommonW, sizeof(CommonW.ContAE));      
      Win->CrAedit(&CommonW.ContAE.ObjDTA,CommonW.ContAE.frame_type);     
    }      
  }
  FileClose(iFileHandle);     
  return(true);  
}  

//--------------------open configuration file-----------------------------
union{
  TConfigFileSP ConfigFileSP;
  TAioChannel AioChannel;
  TDioChannel DioChannel;
  TContAI_USO ContAI_USO;
  TContDIDO_USO ContDIDO_USO;
  TContMUK_USO ContMUK_USO;  
  TContLO1111_USO ContLO1111_USO;
}CommonC;

bool OpenConfigFile(char *PrjFile)
{  
  char nmb_AI_moduls,nmb_DIDO_moduls,
       nmb_MUK_moduls,nmb_LO1111_moduls;  
  
  pTFileHndl iFileHandle = OpenFile(PrjFile,f_rd); 
  if(!iFileHandle){
    char *err_b = new char[25];memset(err_b,0,sizeof(err_b));
    char lens = sprintf(err_b, "%s not fond",PrjFile);  
    OutMsg(err_b, lens,1);  
    delete[] err_b;   
    return(false);
  }
  FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.ConfigFileSP));
  // set port mode
  SetPortMode(pCOM1,CommonC.ConfigFileSP.PortMode.P1_MODE); 
  SetPortMode(pCOM2,CommonC.ConfigFileSP.PortMode.P2_MODE);
  SetPortMode(pCOM3,CommonC.ConfigFileSP.PortMode.P3_MODE);
  SetPortMode(pCOM4,CommonC.ConfigFileSP.PortMode.P4_MODE);      
  //
  nmb_AI_moduls   = CommonC.ConfigFileSP.nmb_AI_moduls;
  nmb_DIDO_moduls = CommonC.ConfigFileSP.nmb_DIDO_moduls;
  nmb_MUK_moduls  = CommonC.ConfigFileSP.nmb_MUK_moduls;
  nmb_LO1111_moduls = CommonC.ConfigFileSP.nmb_LO1111_moduls;
  
  // create AI module 
  for(char i=0;i<nmb_AI_moduls;i++){
    //TAI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTAIOChannel ptr,unsigned short USOpoint);
    FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.ContAI_USO));    
    CUART_BASE_ptr CUART_BASE = GetPortPTR(CommonC.ContAI_USO.PortN);
    if(!CUART_BASE)continue;
    char numb_ai_ch = CommonC.ContAI_USO.ChanN;
    pTAIOChannel pAIOChannel  = new TAioChannel[numb_ai_ch];
    new TAI_USO(CUART_BASE,CommonC.ContAI_USO.Adress,numb_ai_ch,
                                            pAIOChannel,CommonC.ContAI_USO.USOpoint);
    for(char i=0;i<numb_ai_ch;i++){
      FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.AioChannel));
      *(pAIOChannel+i) = CommonC.AioChannel;
    }                                                 
  }
  // create DI/DO module 
  for(char i=0;i<nmb_DIDO_moduls;i++){
    //TDI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTDIOChannel ptr,unsigned short USOpoint);
    FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.ContDIDO_USO));    
    CUART_BASE_ptr CUART_BASE = GetPortPTR(CommonC.ContDIDO_USO.PortN);
    if(!CUART_BASE)continue;
    char numb_dido_ch = CommonC.ContDIDO_USO.ChanN;
    pTDIOChannel pDIOChannel  = new TDioChannel[numb_dido_ch];
    new TDI_USO(CUART_BASE,CommonC.ContDIDO_USO.Adress,numb_dido_ch,
                                            pDIOChannel,CommonC.ContDIDO_USO.USOpoint);
    for(char i=0;i<numb_dido_ch;i++){
      FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.DioChannel));
      *(pDIOChannel+i) = CommonC.DioChannel;
    }                     
  } 
  // create MUK module     
  for(char i=0;i<nmb_MUK_moduls;i++){
     //TMUK_USO(CUART_BASE_ptr pPORT,unsigned char address,unsigned short USOpoint,unsigned short InfoBlkPoint,unsigned short ComBlkPoint);  
     FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.ContMUK_USO));     
     CUART_BASE_ptr CUART_BASE = GetPortPTR(CommonC.ContMUK_USO.PortN);
     if(!CUART_BASE)continue;     
     new TMUK_USO(CUART_BASE,CommonC.ContMUK_USO.Adress,CommonC.ContMUK_USO.USOpoint,
                                 CommonC.ContMUK_USO.InfoBlkPoint,CommonC.ContMUK_USO.ComBlkPoint);  
  }  
  // create LO1111 module     
  for(char i=0;i<nmb_LO1111_moduls;i++){
     //TLO1111_USO(CUART_BASE_ptr pPORT,char address,unsigned short USOpoint,unsigned short PPointNumb,unsigned short SPointNumb);
     FileRead(iFileHandle,(char*)&CommonC,sizeof(CommonC.ContLO1111_USO));     
     CUART_BASE_ptr CUART_BASE = GetPortPTR(CommonC.ContLO1111_USO.PortN);
     if(!CUART_BASE)continue;     
     new TLO1111_USO(CUART_BASE,CommonC.ContLO1111_USO.Adress,CommonC.ContLO1111_USO.USOpoint,
                     CommonC.ContLO1111_USO.PowerPointNumb,CommonC.ContLO1111_USO.StealPointNumb);  
  }   
  
  //
  FileClose(iFileHandle);     
  return(true);  
}  


bool OpenEMODFile(char *PrjFile)
{  
  char TotalMod = 0;
  pTFileHndl iFileHandle = OpenFile(PrjFile,f_rd); 
  if(!iFileHandle){
    char *err_b = new char[25];memset(err_b,0,sizeof(err_b));
    char lens = sprintf(err_b, "%s not fond",PrjFile);  
    OutMsg(err_b, lens,1);  
    delete[] err_b;   
    return(false);
  }
  //read link-file header
  FileRead(iFileHandle,&TotalMod,sizeof(TotalMod));  
  if(!TotalMod){FileClose(iFileHandle);return(false);}
  TMODHEADER CMODHEADER;
  unsigned long offset = sizeof(TotalMod)+ TotalMod*sizeof(TMODHEADER);
  for(char ModN=0;ModN<TotalMod;ModN++){     
    //TEMOD(CUART_BASE_ptr pCOMPORT,unsigned char addr,unsigned short point=0,unsigned long FileOffset=0)
    FileRead(iFileHandle,(char*)&CMODHEADER,sizeof(TMODHEADER));
    CUART_BASE_ptr CUART_BASE = GetPortPTR(CMODHEADER.PortN);
    if(!CUART_BASE)continue;
    new TEMOD(CUART_BASE,CMODHEADER.Adress,CMODHEADER.EMODpoint,offset,CMODHEADER.RRecNumb,CMODHEADER.WRecNumb);
    offset+=(CMODHEADER.RRecNumb*sizeof(TLINKREC)+CMODHEADER.WRecNumb*sizeof(TLINKREC));
    switch (CMODHEADER.PortN){
      case 1   : {SetPortMode(pCOM1,USO);break;}
      case 2   : {SetPortMode(pCOM2,USO);break;}
      case 3   : {SetPortMode(pCOM3,USO);break;}
      case 4   : {SetPortMode(pCOM4,USO);break;}            
      default  : {break;}      
    }                                          
  } 
  FileClose(iFileHandle);     
  return(true);  
}  

bool OpenCodeFile(char *PrjFile)
{
  TCodeFileSP CodeFileSP;  
  
  pTFileHndl iFileHandle = OpenFile(PrjFile,f_rd); 
  if(!iFileHandle){
    char *err_b = new char[25];memset(err_b,0,sizeof(err_b));
    char lens = sprintf(err_b, "%s not fond",PrjFile);  
    OutMsg(err_b, lens,1); 
    delete[] err_b;
    return(false);   
  }
  //read unit nubmer from code file 
  FileRead(iFileHandle,(char*)&CodeFileSP,sizeof(CodeFileSP));  
  if(CodeFileSP.use_tab_numb){
    if(CodeFileSP.nmb_unit_tab1){
      pTCUNIT C_Unit= new TCUNIT[CodeFileSP.nmb_unit_tab1];
      //fill code succession                                   
      for(char i=0;i<CodeFileSP.nmb_unit_tab1;i++){
        FileRead(iFileHandle,(char*)(C_Unit+i),sizeof(TCUNIT));       
      }
      pC_CODE1 = new C_CODE(C_Unit, CodeFileSP.nmb_unit_tab1);    
    }
    if(CodeFileSP.nmb_unit_tab2){
      pTCUNIT C_Unit= new TCUNIT[CodeFileSP.nmb_unit_tab2];
      //fill code succession                                   
      for(char i=0;i<CodeFileSP.nmb_unit_tab2;i++){
        FileRead(iFileHandle,(char*)(C_Unit+i),sizeof(TCUNIT));       
      }
      pC_CODE2 = new C_CODE(C_Unit, CodeFileSP.nmb_unit_tab2);        
    }
    if(CodeFileSP.nmb_unit_tab3){
      pTCUNIT C_Unit= new TCUNIT[CodeFileSP.nmb_unit_tab3];
      //fill code succession                                   
      for(char i=0;i<CodeFileSP.nmb_unit_tab3;i++){
        FileRead(iFileHandle,(char*)(C_Unit+i),sizeof(TCUNIT));       
      }
      pC_CODE3 = new C_CODE(C_Unit, CodeFileSP.nmb_unit_tab3);       
    }
    if(CodeFileSP.nmb_unit_tab4){
      pTCUNIT C_Unit= new TCUNIT[CodeFileSP.nmb_unit_tab4];
      //fill code succession                                   
      for(char i=0;i<CodeFileSP.nmb_unit_tab4;i++){
        FileRead(iFileHandle,(char*)(C_Unit+i),sizeof(TCUNIT));       
      }
      pC_CODE4 = new C_CODE(C_Unit, CodeFileSP.nmb_unit_tab4);       
    }
    if(CodeFileSP.nmb_unit_tab5){
      pTCUNIT C_Unit= new TCUNIT[CodeFileSP.nmb_unit_tab5];
      //fill code succession                                   
      for(char i=0;i<CodeFileSP.nmb_unit_tab5;i++){
        FileRead(iFileHandle,(char*)(C_Unit+i),sizeof(TCUNIT));       
      }
      pC_CODE5 = new C_CODE(C_Unit, CodeFileSP.nmb_unit_tab5);       
    }                
  }
      
  FileClose(iFileHandle);  
  return(true);     
}

bool OpenIOBITFile(char *PrjFile)
{
  pTFileHndl iFileHandle = OpenFile(PrjFile,f_rd); 
  if(!iFileHandle){
    char *err_b = new char[25];memset(err_b,0,sizeof(err_b));
    char lens = sprintf(err_b, "%s not fond",PrjFile);  
    OutMsg(err_b, lens,1); 
    delete[] err_b;
    return(false);   
  }
  //read unit nubmer from code file 
  char TtlREC = 0;
  FileRead(iFileHandle,&TtlREC,sizeof(TtlREC));  

  if(TtlREC){
    pTIO_BIT IO_BIT= new TIO_BIT[TtlREC];
    if(!IO_BIT){FileClose(iFileHandle);return(false);}
    //fill code succession                                   
    for(char i=0;i<TtlREC;i++){
      FileRead(iFileHandle,(char*)(IO_BIT+i),sizeof(TIO_BIT));       
    }
    pIOBITDATA = new TIOBIT(IO_BIT, TtlREC);    
  }
   
  FileClose(iFileHandle);  
  return(true);     
}



void OutMsg(char *label, short length, long timer)
{  
  memset(CommonW.WorkFileSP.WinData.ObjDTA.Caption,0,sizeof(CommonW.WorkFileSP.WinData.ObjDTA.Caption));
  sprintf(CommonW.WorkFileSP.WinData.ObjDTA.Caption, "warning");
  CommonW.WorkFileSP.WinData.ObjDTA.Font = 8; CommonW.WorkFileSP.WinData.ObjDTA.Top = 0; 
  CommonW.WorkFileSP.WinData.ObjDTA.Left = 0; CommonW.WorkFileSP.WinData.ObjDTA.Hight = 79; 
  CommonW.WorkFileSP.WinData.ObjDTA.Width = 159;   
  C_WIN_prt Win = CreateWin(&(CommonW.WorkFileSP.WinData.ObjDTA));
  
  if(Win){
    //CrButton(ptrTVisualObj ButDTA, char butKind, void* pPoint, char *FileName, char SetVal);     
    memset(CommonW.ContBut.ObjDTA.Caption,0,sizeof(CommonW.ContBut.ObjDTA.Caption));
    sprintf(CommonW.ContBut.ObjDTA.Caption, "Exit");
    CommonW.ContBut.ObjDTA.Font  = 8;  CommonW.ContBut.ObjDTA.Top   = 62;
    CommonW.ContBut.ObjDTA.Left  = 95; CommonW.ContBut.ObjDTA.Hight = 13;
    CommonW.ContBut.ObjDTA.Width = 60; CommonW.ContBut.Kind = bkClose;  
    Win->CrButton(&CommonW.ContBut.ObjDTA,CommonW.ContBut.Kind,NULL,NULL,0); 

    //CrLabel(ptrTVisualObj ButDTA);    
    CommonW.ContL.ObjDTA.Font = 8; CommonW.ContL.ObjDTA.Top = 19;
    CommonW.ContL.ObjDTA.Left = 3; CommonW.ContL.ObjDTA.Hight = 8; CommonW.ContL.ObjDTA.Width = 150;  
    short out_length=0;
    while(out_length<(length)){
      memset(CommonW.ContL.ObjDTA.Caption,0,sizeof(CommonW.ContL.ObjDTA.Caption));
      memcpy(CommonW.ContL.ObjDTA.Caption,label+out_length,18);
      Win->CrLabel(&CommonW.ContL.ObjDTA);
      CommonW.ContL.ObjDTA.Top += 11;
      out_length += 18;  
      if(out_length>72)break;  
    }
    // timer  
    if(timer){
      Sleep(timer*1000);
      DestroyWin(); 
      return; 
    } 
  } 
}  

//------------------keypad processing--------------------------------  
void OpenWindow_Ai(TAparam_prt AParam)
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "aedit.dpf");
  if(!OpenPjgFile(file_b)){return;}  
  // search bkYes button & set button propertis
  if(!WinList.LastWind->ObjList.ObjCounter)return;
  TObject_ptr pVObj = WinList.LastWind->ObjList.FirstObj;
  while(pVObj){
    if(pVObj->ObjID == BUTTON){
      if(((TButton_prt)pVObj)->Kind == bkYes){
       ((TButtonYes_prt)pVObj)->pAPoint = AParam->pAPointW;
       ((TButtonYes_prt)pVObj)->AEdit   = ((TAedit_prt)(WinList.LastWind->ObjList.LastObj));
      }     
    }  
    pVObj = pVObj->NextObj;
  } 
}

void OpenWindow_Di(pTDPOINT pPoint,char SetValue) 
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "ack.dpf");
  if(!OpenPjgFile(file_b)){return;}
  //set button propertis
  if(WinList.LastWind->ObjList.FirstObj->ObjID == BUTTON){
    if(((TButton_prt)(WinList.LastWind->ObjList.FirstObj))->Kind == bkYes){
      ((TButtonYes_prt)(WinList.LastWind->ObjList.FirstObj))->pDPoint = pPoint;
      ((TButtonYes_prt)(WinList.LastWind->ObjList.FirstObj))->SetValue_D = SetValue;
    }  
  }  
}

void ButtonClick(TButton_prt Button)
{
  Button->KeyDown();

  switch (Button->Kind){           
   case bkClose    : {Button->KeyUp();DestroyWin();return;}   
   case bkShWind   : {OpenPjgFile(((TButtonSW_prt)Button)->WindName);break;}        
   case bkChDpoint : {OpenWindow_Di(((TButtonDP_prt)Button)->pDPoint, ((TButtonDP_prt)Button)->SetValue);break;}                
   case bkYes      : {Button->KeyUp();DestroyWin();return;}       
   case bkNo       : {Button->KeyUp();DestroyWin();return;}   
   default   : {break;}
  }      
  Button->KeyUp();  
}

void BMPClick(TBitMap_prt BmpImage)
{
  if(BmpImage->WindName[0])OpenPjgFile(BmpImage->WindName);     
}

void PadProcessor()
{
  if(SMODE != NORMAL)return;
  //key press processor
  char KeyCode = KEYPAD.getlastchar();
  if(KeyCode){
      //on lcd ligth
      OnLcdLight();
      lOffLigthLCD.LineSleep(60000); 
      //
      if(!WinList.WindCounter)return; 
      TObject_ptr pVObj = WinList.LastWind->GetSelectObj();
      switch (KeyCode){
        case 0x0D : {                                                                   // Enter
                      switch (pVObj->ObjID){
                        case BUTTON : {ButtonClick((TButton_prt)pVObj);break;}       
                        case AEDIT  : {break;} 
                        case APOINT : {OpenWindow_Ai((TAparam_prt)pVObj);break;}  
                        case DPOINT : {break;} 
                        case BITMAP : {BMPClick((TBitMap_prt)pVObj);break;} 
                        case LABEL  : {break;} 
                        default   : {break;}
                      }
                      break;
                    }       
        case 0x1B : {DestroyWin();break;}                                               // ESC
        case 0x10 : {WinList.LastWind->SetFocusNext();break;}                           // Right
        case 0x11 : {if(pVObj->ObjID == AEDIT) ((TAedit_prt)pVObj)->KeyPress(KeyCode);  // Left
                      else WinList.LastWind->SetFocusPred();
                     break;
                    } 
        case 0x1E : {WinList.LastWind->SetFocusPred();break;}                           // Up
        case 0x1F : {WinList.LastWind->SetFocusNext();break;}                           // Down
        default   : {
                      if(pVObj->ObjID == AEDIT)((TAedit_prt)pVObj)->KeyPress(KeyCode);  // Data key                 
                      break;
                    }
                                    
      }      
     KEYPAD.clrkeybuf(); 
    }
}

//  lOffLigthLCD.LineSleep(10000); 


//----------------------------Init function----------------------------------
void StrtInfoBLK()
{ 
  lOffLigthLCD.LineSleep(60000); 
  VLCD.Clr();  
  InfoBLK(NULL);
  Sleep(4000); 
  VLCD.Clr();
}

void InitProcess()
{        
  if(STATDTA.set_dta==BUSY){
    if(STATDTA.COM1.lCOMspeed) COM1.UARTreinit(HOST,STATDTA.COM1.lCOMspeed,STATDTA.COM1.cCOMparity);
     else COM1.UARTreinit(HOST,COM1.COM_SETUP.lCOMspeed,COM1.COM_SETUP.cCOMparity);
    if(STATDTA.COM2.lCOMspeed) COM2.UARTreinit(HOST,STATDTA.COM2.lCOMspeed,STATDTA.COM2.cCOMparity);
     else COM2.UARTreinit(HOST,COM2.COM_SETUP.lCOMspeed,COM2.COM_SETUP.cCOMparity);  
    if(STATDTA.COM3.lCOMspeed) COM3.UARTreinit(HOST,STATDTA.COM3.lCOMspeed,STATDTA.COM3.cCOMparity);
     else COM3.UARTreinit(HOST,COM3.COM_SETUP.lCOMspeed,COM3.COM_SETUP.cCOMparity);
    if(STATDTA.COM4.lCOMspeed) COM4.UARTreinit(HOST,STATDTA.COM4.lCOMspeed,STATDTA.COM4.cCOMparity);
     else COM4.UARTreinit(HOST,COM4.COM_SETUP.lCOMspeed,COM4.COM_SETUP.cCOMparity);
  }else{
    //set COM1
    STATDTA.COM1.lCOMspeed   = COM1.COM_SETUP.lCOMspeed; 
    STATDTA.COM1.cCOMparity  = COM1.COM_SETUP.cCOMparity;
    STATDTA.COM1.cCOMbitnumb = COM1.COM_SETUP.cCOMbitnumb;    
    //set COM2
    STATDTA.COM2.lCOMspeed   = COM2.COM_SETUP.lCOMspeed; 
    STATDTA.COM2.cCOMparity  = COM2.COM_SETUP.cCOMparity;
    STATDTA.COM2.cCOMbitnumb = COM2.COM_SETUP.cCOMbitnumb;    
    //set COM3
    STATDTA.COM3.lCOMspeed   = COM3.COM_SETUP.lCOMspeed; 
    STATDTA.COM3.cCOMparity  = COM3.COM_SETUP.cCOMparity;
    STATDTA.COM3.cCOMbitnumb = COM3.COM_SETUP.cCOMbitnumb;    
    //set COM4
    STATDTA.COM4.lCOMspeed   = COM4.COM_SETUP.lCOMspeed; 
    STATDTA.COM4.cCOMparity  = COM4.COM_SETUP.cCOMparity;
    STATDTA.COM4.cCOMbitnumb = COM4.COM_SETUP.cCOMbitnumb;  
    //set address
    STATDTA.address = 1; 
    //set flg
    STATDTA.set_dta = BUSY;   

    if(STATDTA.COM1.lCOMspeed) COM1.UARTreinit(HOST,STATDTA.COM1.lCOMspeed,STATDTA.COM1.cCOMparity);
     else COM1.UARTreinit(HOST,COM1.COM_SETUP.lCOMspeed,COM1.COM_SETUP.cCOMparity);
    if(STATDTA.COM2.lCOMspeed) COM2.UARTreinit(HOST,STATDTA.COM2.lCOMspeed,STATDTA.COM2.cCOMparity);
     else COM2.UARTreinit(HOST,COM2.COM_SETUP.lCOMspeed,COM2.COM_SETUP.cCOMparity);  
    if(STATDTA.COM3.lCOMspeed) COM3.UARTreinit(HOST,STATDTA.COM3.lCOMspeed,STATDTA.COM3.cCOMparity);
     else COM3.UARTreinit(HOST,COM3.COM_SETUP.lCOMspeed,COM3.COM_SETUP.cCOMparity);
    if(STATDTA.COM4.lCOMspeed) COM4.UARTreinit(HOST,STATDTA.COM4.lCOMspeed,STATDTA.COM4.cCOMparity);
     else COM4.UARTreinit(HOST,COM4.COM_SETUP.lCOMspeed,COM4.COM_SETUP.cCOMparity);   
  }
  // user inicialization
  UserInitialization();
  // wait responde from slave MCPU
/*
  while(MASTER_BLOCK.cMasterInit != 0xAE){
    MASTER_BLOCK.cMasterInit = 0xAE; 
    __delay_cycles(100);
  }  
*/  
}
  
void LoadConfiguration()
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "uso.cnf");
  OpenConfigFile(file_b);
}

void LoadC_CODE()
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "code.ftb");
  OpenCodeFile(file_b);
}

void LoadIO_BIT()
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "IOBit.cnf");
  OpenIOBITFile(file_b);
}
  
void LoadEMODfile()
{
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "ExtMod.lnk");
  OpenEMODFile(file_b);
}

void StartWin()
{    
  char file_b[12];memset(file_b,0,sizeof(file_b));
  sprintf(file_b, "main.dpf");
  OpenPjgFile(file_b);
}

// USO scaning & processing
void UsoProcessor()
{     
  if(USO_List.ModuleCounter){ 
     pTUSO_Module pActiveUso = USO_List.FirstModule;
     while(pActiveUso){
      //scan USO module         
      pActiveUso->ScanStart();          
      pActiveUso = pActiveUso->pNext;        
     }               
  } 
}

// External module scaning & processing
void ModProcessor()
{
  if(EMOD_List.ModuleCounter){ 
     pTEMOD pEMOD = EMOD_List.FirstModule;
     while(pEMOD){
      //scan USO module         
      pEMOD->ScanStart();          
      pEMOD = pEMOD->pNext;        
     }               
  } 
}

void PointArchive()
{
  char hour = ((pTIME->Hour)>>4)*10 + (pTIME->Hour & 0x0F); 
  char min =  ((pTIME->Min)>>4)*10 + (pTIME->Min & 0x0F); 
  static bool FirstCicle = true;
  static pTFileHndl pFileHndl = NULL;
  static unsigned short ttl_time = hour*60 + min;
  char a_file[12]; memset(a_file,0,sizeof(a_file)); sprintf(a_file, "AParh.arh");  
  char ttl_record = 3;
  // archive file present ?
  TFilePoint FilePoint = FileExist(a_file);
  if(!(FilePoint.sector || FilePoint.offset)){return;}
  //open archive file in first cycle
  if(FirstCicle){
    pFileHndl = OpenFile(a_file, f_wr);
    FirstCicle = false;
  }
  //data write
  if(pFileHndl){
    unsigned short new_time = hour*60 + min;     
    if(new_time!=ttl_time){                      
      //open archeve file
      
      //read data from file
      
      //write data
      unsigned long position = (ttl_record*8+2)*new_time;        
      TTIME_COMPACT Time; Time.Min = min; Time.Hour = hour; Time.Data = pTIME->Data;
      FileSeek(pFileHndl, position, SEEK_SET);
      FileWrite(pFileHndl, (char*)&Time, sizeof(Time));  
      for(unsigned short d_numb=0;d_numb<ttl_record;d_numb++){
        FileWrite(pFileHndl, (char*)&d_numb, sizeof(d_numb));
        FileWrite(pFileHndl, (char*)(TABEL_APOINT+d_numb), sizeof(TAPOINT)); 
        if(d_numb==20)break;
      }      
      //close archive file              
      ttl_time = new_time;
    } // if(new_time!=ttl_time)...
  } //if(pFileHndl)...
}

//----------------------------User function------------------------
void UserInitialization()
{
  //start time & data
  unsigned long *tmp_l = (unsigned long *)&(TABEL_APOINT[4].value);
  *tmp_l = GenLfrom2W(GenWfrom2B(pTIME->Min,pTIME->Sec), GenWfrom2B(pTIME->Hour,0));      
  tmp_l = (unsigned long *)&(TABEL_APOINT[5].value);
  *tmp_l = GenLfrom2W(GenWfrom2B(pTIME->Month,pTIME->Data), GenWfrom2B(pTIME->Year,0));                                                                
}

void UserAlgoritm()
{
  // fill system point 
  TABEL_APOINT[0].value = DATA.fTerm;       // system temperature
  TABEL_APOINT[1].value = DATA.fU5V;        // system supply 5V
  TABEL_APOINT[2].value = DATA.fU24v;       // system supply 24V
  if((TABEL_APOINT[0].value < -20) || (TABEL_APOINT[0].value > 50))TABEL_APOINT[0].status = STATUS_ALARM;
   else TABEL_APOINT[0].status = STATUS_RELIABLE; 
  if((TABEL_APOINT[1].value < 4.5) || (TABEL_APOINT[1].value > 6))TABEL_APOINT[1].status = STATUS_ALARM;
   else TABEL_APOINT[1].status = STATUS_RELIABLE; 
  if((TABEL_APOINT[2].value < 20) || (TABEL_APOINT[2].value > 26))TABEL_APOINT[2].status = STATUS_ALARM;
   else TABEL_APOINT[2].status = STATUS_RELIABLE;       
  // system time 
  TABEL_APOINT[3].value = (unsigned long)TIMETICK; TABEL_APOINT[3].status = STATUS_RELIABLE;    
  // system time & data
  unsigned long *tmp_l = (unsigned long *)&(TABEL_APOINT[6].value);
  *tmp_l = GenLfrom2W(GenWfrom2B(pTIME->Min,pTIME->Sec), GenWfrom2B(pTIME->Hour,0));        
  tmp_l = (unsigned long *)&(TABEL_APOINT[7].value);
  *tmp_l = GenLfrom2W(GenWfrom2B(pTIME->Month,pTIME->Data), GenWfrom2B(pTIME->Year,0));  
  // stack & heap control 
  tmp_l = (unsigned long *)&(TABEL_APOINT[8].value); *tmp_l = GenLfrom2W(RStackKontrol(), CStackKontrol()); // Code stack control & Data stack control
  TABEL_APOINT[8].status = STATUS_RELIABLE; 
  tmp_l = (unsigned long *)&(TABEL_APOINT[9].value); *tmp_l = (unsigned long)HeapKontrol();
  TABEL_APOINT[9].status = STATUS_RELIABLE; 
  //code succession
  if(pC_CODE1){pC_CODE1->CodeProcessor();}                                                                
  if(pC_CODE2){pC_CODE2->CodeProcessor();}        
  if(pC_CODE3){pC_CODE3->CodeProcessor();}        
  if(pC_CODE4){pC_CODE4->CodeProcessor();}        
  if(pC_CODE5){pC_CODE5->CodeProcessor();}   
  if(pIOBITDATA){pIOBITDATA->IOBITRecProcessor();}     

  lWorkAlgoritm.LineSleep(50); 
} 


