#ifndef _EMOD
#define _EMOD

#include "EMODDef.h"

extern TAPOINT TABEL_APOINT[]; 
extern TDPOINT TABEL_DPOINT[];

//---------------------------------common list's---------------------------------------------
class TEMOD;
typedef TEMOD *pTEMOD;

struct TEMOD_List{                  // sructure for list
  pTEMOD   FirstModule;
  pTEMOD   LastModule;
  unsigned char ModuleCounter;
};
 
//-----------------------CREATE CLASS FOR External module processing-------------------------

class TEMOD{
 public:
   //common data 
   pTEMOD  pNext;                     //
   unsigned char Address;             // device address
   unsigned char ComErr;              // read/write link error
   unsigned char CmdCount;            // write attempt count   
   unsigned char EMODStatus;          // status point 
   unsigned short EMODPointNumb;      // storage in Dpoint USO status       
   CUART_BASE_ptr pCOM;               // comumunication port pointer
   //External module data 
   unsigned long OffsetInFile;        // offset in link file
   unsigned char RRecNumb;            // number of read point
   unsigned char WRecNumb;            // number of write point

   TEMOD(CUART_BASE_ptr pCOMPORT,unsigned char addr,unsigned short point=0,
           unsigned long FileOffset=0,unsigned char RNumb=0,unsigned char WNumb=0);
   ~TEMOD(){}

   void AskRegister(unsigned short FReg,unsigned short LReg,pTLINKREC pLINKRECARR);
   void WrtRegister(pTLINKREC pLINKREC);
   bool ScanStart();
   void ScanEnd(){}      
 private:                                                        
   void AddToList();                 // add virtual timer in work list   
};

#endif /*_EMOD*/

