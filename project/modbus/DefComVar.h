#ifndef _COMMONVARIABLE
#define _COMMONVARIABLE

#include "LCDDef.h"
#include "FlashDef.h" 

typedef unsigned long  DWORD;
typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef short INT;
typedef long LONG;

//simafor blok
struct TSysFlags{
    unsigned char error_memory;
    unsigned char error_share_memory;    
    unsigned char error_LCD;      
    unsigned char error_keypad;
    unsigned char div_zero;   
    unsigned char repletion_C_stack;   
    unsigned char repletion_R_stack; 
    unsigned char internal_repletion;            
 };

struct TProcessFlags{
    unsigned char I_UART1_RxD;
    unsigned char I_UART1_TxD;
    unsigned char I_UART2_RxD;
    unsigned char I_UART2_TxD;  
    unsigned char I_I2C_RxD;
    unsigned char I_I2C_TxD;         
 };

struct TTIME{
  char Calibration;
  char Sec;
  char Min;
  char Hour;
  char Day;
  char Data;
  char Month;
  char Year;  
 }; typedef TTIME *prtTTIME;

struct TTIME_COMPACT{
   unsigned short Min  : 6,
                  Hour : 5,
                  Data : 5;   
 }; typedef TTIME_COMPACT *pTTIME_COMPACT;

struct TIO_UART{
   unsigned char  cOPack_trans;
   unsigned long  lOPack_numb;
   unsigned short sOPack_size;
   unsigned char  aOPack[500];
   unsigned char  cIPack_rec;
   unsigned long  lIPack_numb;
   unsigned short sIPack_size;
   unsigned char  aIPack[500];
 }; typedef TIO_UART *prtTIO_UART;
 
struct TIO_I2C{
   unsigned char  cOPack_trans;
   unsigned long  lOPack_numb;
   unsigned char  cOPack_size;
   unsigned char  aOPack[50];
   unsigned char  cIPack_rec;
   unsigned long  lIPack_numb;
   unsigned char  cIPack_size;
   unsigned char  aIPack[50];
   unsigned char  IO_err;
}; typedef TIO_I2C *prtTIO_I2C;
 
struct TDATA_BLOCK{
   unsigned char cNew_press;
   unsigned char cKey_code;
   float fTerm;  
   float fU5V;
   float fU24v; 
 }; typedef TDATA_BLOCK *ptrTDATA_BLOCK;
 
struct TCONTROL_BLOCK{
   unsigned char cLightOn;
   unsigned char cLCDControl;
   unsigned char cHotDog;
   unsigned char SvdControl; 
 }; typedef TCONTROL_BLOCK *ptrTCONTROL_BLOCK;

struct TCOM_SETUP{
   unsigned long lCOMspeed; 
   unsigned char cCOMbitnumb;
   unsigned char cCOMparity;  
 }; typedef TCOM_SETUP *ptrTCOM_SETUP;
 
struct TMASTER_SETUP{
   unsigned char cMasterInit; 
   TCOM_SETUP COM1_SET;   
   TCOM_SETUP COM2_SET;  
 }; typedef TMASTER_SETUP *ptrTMASTER_SETUP;

struct TVIDEO_BUFFER{
   unsigned short sVBUFsize;
   unsigned char  VBuf[3840];
 }; typedef TVIDEO_BUFFER *ptrTVIDEO_BUFFER;

struct TFLGS_BUFFER{
   unsigned char ComLatch;
   unsigned char I2CLatch;
   unsigned char DataLatch;   
   unsigned char ControlLatch;      
 }; typedef TFLGS_BUFFER *ptrTFLGS_BUFFER;
 
struct TINT_BUFFER{ 
   unsigned char INT_L;
   unsigned char INT_R;
 }; typedef TINT_BUFFER *ptrTINT_BUFFER;
 
//system variables
extern __no_init volatile TSysFlags SysFlags;
extern __no_init volatile TProcessFlags ProcFlags;

//--------------------------------------
struct TFlsSector{
   unsigned char  FileName[size_data_sec];
   unsigned short NextS;
 }; typedef TFlsSector *pTFlsSector; 
 
struct TSBLK{
   unsigned short Signature;
   unsigned short TotalSector;
   unsigned short BusySector;
   unsigned short LastFreeSector;       // old
   unsigned short StrtRoot; 
   unsigned short CRC;    
 }; typedef TSBLK *pTSBLK;

struct TROOTCAT{
   unsigned short Signature;
   unsigned short TotalRecord;
   unsigned short BlkNumber;          // old
   unsigned short NextBlkPtr;         // old
   unsigned short BlkCRC;     
 }; typedef TROOTCAT *pTROOTCAT;

struct TFileRecord{
   unsigned char  FileName[21];
   unsigned short RecTime;
   unsigned short RecData;
   unsigned short StrtSector;     
   unsigned long  FileSize;
   unsigned short FileCRC; 
 }; typedef TFileRecord *pTFileRecord; 

struct TFileHndl{
   char mode;
   unsigned long Fpoiner;
   TFileRecord FRec;
}; typedef TFileHndl *pTFileHndl;

struct TFileListObj{
  TFileHndl FileHndl;
  TFileListObj *NextFileObj;
}; typedef TFileListObj *pTFileListObj;

struct TFileListStrc{                   // sructure for file list 
    pTFileListObj pStrtFileObj;
    pTFileListObj pLstFileObj;
    unsigned char FileObjCntr;
}; typedef TFileListStrc *pTFileListStrc;
 
struct TFilePoint{                   // sructure for filepoint
      unsigned short sector;
      unsigned short offset;
}; typedef TFilePoint *pTFilePoint;

//--------------------------------------

struct TBlock{
  char Sign;        
  char Type;       
  unsigned short Size;      
}; typedef TBlock *pTBlock;

struct TFileHeader{
  char FileName[80]; 
  unsigned short FileDate;
  unsigned short FileTime;
  unsigned long  FileSize;
  unsigned short FileCRC;
  unsigned short CRC; 
}; typedef TFileHeader *pTFileHeader;

struct TFileData{
  unsigned short BlockN;    
  unsigned short BlockCount; 
  char Data[264]; 
  unsigned short CRC;       
}; typedef TFileData *pTFileData;

//--------------------------------------
struct TAPOINT{
  unsigned short status;
  float value;
}; typedef TAPOINT *pTAPOINT;

struct TDPOINT{
  char status;
  char value;
}; typedef TDPOINT *pTDPOINT; 

struct TSTATDTA{
  char set_dta;       // if structure fill - set_dta = busy
  char address;
  TCOM_SETUP COM1;
  TCOM_SETUP COM2;
  TCOM_SETUP COM3;
  TCOM_SETUP COM4;
}; typedef TSTATDTA *pTSTATDTA; 
//--------------------------------------
struct TVisualObj{
   char Caption[20];
   char Font;
   unsigned short Top;
   unsigned short Left;
   unsigned short Hight;
   unsigned short Width;            
 }; typedef TVisualObj *ptrTVisualObj; 
 
struct TContBut{
  TVisualObj ObjDTA;      // 33 byte
  char Kind;
  char FileName[12];
  char NumbPoint;     
  char SetValue; 
}; typedef TContBut *pTContBut;             // ttl 48 byte

struct TContAP{
  TVisualObj ObjDTA;      // 33 byte
  char NumbPointR;
  char NumbPointW;
  bool frame_type;
  bool modifiable;
  char PLC_node;
}; typedef TContAP *pTContAP;               // ttl 37 byte

struct TContAE{
  TVisualObj ObjDTA;      // 33 byte
  bool frame_type;
}; typedef TContAE *pTContAE;               // ttl 34 byte

struct TContDP{
  TVisualObj ObjDTA;      // 33 byte
  char NumbPoint;
  char LowString[16];
  char HiString[16];
  char PLC_node;
}; typedef TContDP *pTContDP;               // ttl 67 byte

struct TContLabel{
  TVisualObj ObjDTA;                
}; typedef TContLabel *pTContLabel;         // ttl 33 byte

struct TContBitMap{
  TVisualObj ObjDTA;      // 33 byte
  bool modifiable;
  char BMFileName[12];
  char FileName[12];
  char NumbPoint;     
  char InitValue;   
  unsigned short TimeRdrw;
}; typedef TContBitMap *pTContBitMap;       // ttl 62 byte

struct TContWind{
  TVisualObj ObjDTA;    
}; typedef TContWind *pTContWind;           // ttl 33 byte

struct TWorkFileSP{
  char nmb_but;
  char nmb_ap;  
  char nmb_dp;
  char nmb_label;
  char nmb_bitmap;
  char nmb_ae;
  char nmb_tmp[10];
  //----
  TContWind WinData;
  //----
  // BUTTON  * nmb_but
  // A POINT * nmb_ap
  // D POINT * nmb_dp
  // LABEL   * nmb_label
  // BITMAP  * nmb_bitmap
  // AEDIT   * nmb_ae
}; typedef TWorkFileSP *pTWorkFileSP;
//--------------------------------------
struct TPortMode{
  char P1_MODE; 
  char P2_MODE;
  char P3_MODE;
  char P4_MODE;      
}; typedef TPortMode *pTPortMode;             // ttl  byte

struct TContAI_USO{
  char PortN;
  char Adress;
  char ChanN;
  unsigned short USOpoint;
}; typedef TContAP *pTContAP;               // ttl  byte

struct TContDIDO_USO{
  char PortN;
  char Adress;
  char ChanN;  
  unsigned short USOpoint;
}; typedef TContDIDO_USO *pTContDIDO_USO;               // ttl  byte


struct TContMUK_USO{
  char PortN;
  char Adress;
  unsigned short USOpoint;   
  unsigned short InfoBlkPoint;
  unsigned short ComBlkPoint;
}; typedef TContMUK_USO *pTContMUK_USO;           // ttl  byte

struct TContLO1111_USO{
  char PortN;
  char Adress;
  unsigned short USOpoint;   
  unsigned short PowerPointNumb;
  unsigned short StealPointNumb;
}; typedef TContLO1111_USO *pTContLO1111_USO;           // ttl  byte

struct TAioChannel{
   char Number;                 // number of chanel
   char Type;                   // chanel type
   float  MinMid;               // low limit of middle value
   float  MaxMid;               // hi limit of middle value
   float  MinVal;               // low limit of phisical value
   float  MaxVal;               // hi limit of phisical value
   unsigned short Code;         // code from ADC
   float MidValue;              // middle value
   float PhisValue;             // phisical value
   char  ChanelStatus;             // status
   unsigned short MidPointNumb;    // storage point for middle value
   unsigned short ValPointNumb;    // storage point for AI & DI3 chanel
};// ttl 29 byte
typedef TAioChannel *pTAIOChannel;

struct TDioChannel{
   char Type;                   // chanel type
   unsigned short PointNumb;    // storage point for DI3 chanel
};// ttl 3 byte
typedef TDioChannel *pTDIOChannel;

struct TConfigFileSP{
  TPortMode PortMode;
  char nmb_AI_moduls;  
  char nmb_DIDO_moduls;
  char nmb_tmp1_moduls;
  char nmb_tmp2_moduls;
  char nmb_MUK_moduls;
  char nmb_LO1111_moduls;
  char nmb_tmp[9];
  //----
  // ContAI_USO * nmb_AI_moduls
          //TAioChannel AioChannel * numb_ch from ContAI_USO;
  // ContDI16_USO * nmb_DI16_moduls
  // ContDI11_DO5_USO * nmb_DI11_DO5_moduls
  // ContDI16_DO4_USO * nmb_DI16_DO4_moduls
  // ContMUK_USO * nmb_MUK_moduls  
}; typedef TConfigFileSP *pTConfigFileSP;

//---C_CODE block---
struct TCUNIT{  /* bmi */
    char POSITION;
    char COMMAND;
    float OP1;
    float OP2;
};  typedef TCUNIT *pTCUNIT; 

struct TCodeFileSP{
  char use_tab_numb;
  char nmb_unit_tab1;  
  char nmb_unit_tab2;
  char nmb_unit_tab3;
  char nmb_unit_tab4;
  char nmb_unit_tab5;      
}; typedef TCodeFileSP *pTCodeFileSP;

//--------------------------------------
#define  RDOUT     1     // read discret output
#define  RDINP     2     // read discret input
#define  RAINP     4     // read analog input
#define  WDOUT     5     // write discret output

#define  RREG     3      // read array of MB registers 
#define  WARRREG  16     // write array of MB registers  
#define  WREG     6      // write MB register 

struct TMREQ{
  char address; char command; char start_reg_hi; char start_reg_low; 
  char numb_reg_hi; char numb_reg_low; char numb_b;  
  char data[300];
}; typedef TMREQ *pTMREQ;         
  
//request structure 
struct TMWRREG{
  char address; char command; char start_reg_hi; char start_reg_low;   
  unsigned short REG; unsigned short CRC;
 }; typedef TMWRREG *pTMWRREG;
               
struct TMRESP{
  char address; char command; char counter;
  unsigned short OutputBuf[250];
}; typedef TMRESP *pTMRESP;  

struct TRdFlash{
  char addr;  
  char op_n;
  unsigned short Npage;
  unsigned char Data[size_sec];
  unsigned short CRC;
 }; typedef TRdFlash *pTRdFlash; 
 
//*.bmp file structure
struct TBMFH{    /* bmfh */
    unsigned short bfType;
    unsigned long  bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long  bfOffBits;
}; typedef TBMFH *pTBMFH; 

struct TBMIH{    /* bmih */
    unsigned long  biSize;
    unsigned long  biWidth;
    unsigned long  biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    unsigned long  biXPelsPerMeter;
    unsigned long  biYPelsPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImportant;
}; typedef TBMIH *pTBMIH; 

struct TBMI{  /* bmi */
    TBMIH    bmiHeader;
    unsigned long bmiColors[2];
};  typedef TBMI *pTBMI; 

//------------------------------------------------------------------------------
// link record processor
struct TLINKREC{
  unsigned short StrtReg;
  unsigned char  OpType;
  unsigned short NPoint;
}; typedef TLINKREC *pTLINKREC;

struct TMODHEADER{
  char Adress;
  char PortN;
  unsigned short EMODpoint;
  unsigned char RRecNumb;
  unsigned char WRecNumb;
}; typedef TMODHEADER *pTMODHEADER;

//------------------------------------------------------------------------------
// IO Bit
struct TBITFIELD{
   unsigned short BITFIELD;   
 }; typedef TBITFIELD *pTBITFIELD;

struct TIO_BIT{
  unsigned char  DevType;
  unsigned char  LogicType;
  unsigned short NPoint;
  unsigned short NIOBit;
}; typedef TIO_BIT *pTIO_BIT;  

#endif /*_COMMONVARIABLE*/

