#ifndef _USO
#define _USO

#include "USODef.h"

extern TAPOINT TABEL_APOINT[]; 
extern TDPOINT TABEL_DPOINT[];

//---------------------------------common list's---------------------------------------------
// common 
class TUSO_Module;
typedef TUSO_Module *pTUSO_Module;
struct TUso_List{                  // sructure for list
  pTUSO_Module   FirstModule;
  pTUSO_Module   LastModule;
  unsigned char ModuleCounter;
};
// AI uso
class TAI_USO;
typedef TAI_USO *pTAI_USO;
struct TAI_List{
  pTAI_USO   FirstModule;
  pTAI_USO   LastModule;
  unsigned char  AiModuleCounter;
}; 
// MUK uso 
class TMUK_USO;
typedef TMUK_USO *pTMUK_USO;
// DI uso
class TDI_USO;
typedef TDI_USO *pTDI_USO;
 
//-----------------------CREATE CLASS FOR USO processing----------------------------------------

class TUSO_Module {
 public:
   pTUSO_Module  pNext;              //
   unsigned char Address;            // 
   unsigned char ComErr;             //
   unsigned char Channels;           // 
   unsigned char USOStatus;          // status
   unsigned short USOPointNumb;      // storage in Dpoint USO status 
   CUART_BASE_ptr pCOM;              // comumunication port pointer

   TUSO_Module(CUART_BASE_ptr pCOMPORT, unsigned char addr, unsigned char channels, unsigned short point=0);
   ~TUSO_Module(){}

   virtual void ScanStart(void){}
   virtual void ScanEnd(void){}      
 private:                                                        
   void AddToList();                 // add virtual timer in work list   
};

//-----------------------CREATE CLASS FOR AI module processing---------------------------------

class TAI_USO:public TUSO_Module{
  private:  
    void ConvAiToVal();
    void ConvAiToParam();
    void AddToAiList(); 
  public:
    pTAI_USO     pNextAi;
    pTAIOChannel pCh;   
    
    TAI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTAIOChannel ptr,unsigned short USOpoint);
    ~TAI_USO(){}
    bool ScanAI();
    void SetAO();
    virtual void ScanStart() {if(ScanAI()){ConvAiToVal();ConvAiToParam();} SetAO();}
    virtual void ScanEnd(){}                                                                   
};
 

//-----------------------CREATE CLASS FOR DI/DO module processing-----------------------------

class TDI_USO:public TUSO_Module{
  private:  
  public:
    pTDI_USO     pNextDi;
    pTDIOChannel pCh;   
    
    TDI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTDIOChannel ptr,unsigned short USOpoint);
    ~TDI_USO(){}
    void ScanDI();
    void SetDO();
    virtual void ScanStart() {ScanDI(); SetDO();}
    virtual void ScanEnd(){}                                                                   
}; 
//-----------------------CREATE CLASS FOR AI module processing---------------------------------
struct TVALVE{char MUK; char Valve; unsigned short Gear;};

class TMUK_USO:public TUSO_Module
{
  public:   
    unsigned char  LastCommand;         // 
    unsigned char  CmdCount;            //  
    unsigned char  MukMustState;        //  
    TVALVE V;  
    //command blok point
    unsigned short ComBlcPointNumb;     // start command blok for current MUK / 2 word
    unsigned short InfoBlcPointNumb;    // start info blok for current MUK / 2 word
    
    TMUK_USO(CUART_BASE_ptr pPORT,unsigned char address,unsigned short USOpoint,unsigned short InfoBlkPoint,unsigned short ComBlkPoint);
    ~TMUK_USO(){}
    void CLEAR_VALVE(TVALVE *pV);
    void CalcVALVE(char Data[5]);
    void MukAnalise();     
    bool ScanMUK();
    void SetMUK();
    void ResetMUK(){LastCommand = MUK_RESET; CmdCount=0;}
    void OpenMUK (){LastCommand = MUK_OPEN;  CmdCount=0;}
    void CloseMUK(){LastCommand = MUK_CLOSE; CmdCount=0;}   
    virtual void ScanStart(){if(ScanMUK()){MukAnalise();} SetMUK();}
    virtual void ScanEnd(){}                                                               
 };
 
//-----------------------CREATE CLASS FOR OBLIK LO-1111---------------------------------
class TLO1111_USO:public TUSO_Module
{
  public:
    //command blok point
    unsigned short PowerPointNumb;       // point for power value save
    unsigned short StealPointNumb;       // point for steal time save

    TLO1111_USO(CUART_BASE_ptr pPORT,char address,unsigned short USOpoint,unsigned short PPointNumb,unsigned short SPointNumb);
    ~TLO1111_USO(){}
    
    bool ScanLO();
    virtual void ScanStart(){ScanLO();}
    virtual void ScanEnd(){}     
};typedef TLO1111_USO *pTLO1111_USO;
 
#endif /*_USO*/

