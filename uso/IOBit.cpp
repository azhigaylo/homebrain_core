#include "kernel.h"
//------------------------------------------------------------------------

TIOBIT::TIOBIT(pTIO_BIT piIO_BIT, char iRecNumber)
{
  RecNumber = iRecNumber;            // total number of record
  pIO_BIT   = piIO_BIT;              // pointer on first record unit  
  // copy TABEL_OBIT in mirror table
  memcpy((char*)&MTABEL_OBIT,(char*)&TABEL_OBIT,sizeof(TBITFIELD)*(bit_out_number/16));  
}       
    
void TIOBIT::IOBITRecProcessor()
{
  pTIO_BIT pCUR_IO_BIT = pIO_BIT;  
  char CURREC = 0;
  
  while(CURREC<RecNumber){            
    switch (pCUR_IO_BIT->DevType){           
      case  IDEV : {SetResetIBit(pCUR_IO_BIT);break;}    
      case  ODEV : {OBitProc(pCUR_IO_BIT);break;}            
      default    : {break;}
    }             
    CURREC++;
    pCUR_IO_BIT++;    
  }  
  // copy TABEL_OBIT in mirror table
  memcpy((char*)&MTABEL_OBIT,(char*)&TABEL_OBIT,sizeof(TBITFIELD)*(bit_out_number/16));
}      
  
void TIOBIT::SetResetIBit(const pTIO_BIT pIO_BIT)
{
  unsigned short NPoint    = pIO_BIT->NPoint;
  unsigned short NIOBit    = pIO_BIT->NIOBit;  
  //search point of input bit
  unsigned short reg_nmb = NIOBit/16;
  unsigned short pos_in_reg = NIOBit%16;
  
  if(pIO_BIT->LogicType == Direct){
    if(TABEL_DPOINT[NPoint].value){
      //set bit
      TABEL_IBIT[reg_nmb].BITFIELD |= ((unsigned short)0x8000 >> pos_in_reg);  
    }else{
      //reset bit 
      TABEL_IBIT[reg_nmb].BITFIELD &= ~((unsigned short)0x8000 >> pos_in_reg); 
    }
  }else{
    if(TABEL_DPOINT[NPoint].value){
      //reset bit 
      TABEL_IBIT[reg_nmb].BITFIELD &= ~((unsigned short)0x8000 >> pos_in_reg); 
    }else{      
      //set bit
      TABEL_IBIT[reg_nmb].BITFIELD |= ((unsigned short)0x8000 >> pos_in_reg);      
    }    
  }      
}

void TIOBIT::OBitProc(const pTIO_BIT pIO_BIT)
{
  unsigned short NPoint    = pIO_BIT->NPoint;
  unsigned short NIOBit    = pIO_BIT->NIOBit;  
  //search point of input bit
  unsigned short reg_nmb = NIOBit/16;
  unsigned short pos_in_reg = NIOBit%16; 
  //change analise
  unsigned short dir_v  = TABEL_OBIT[reg_nmb].BITFIELD & (((unsigned short)0x8000 >> pos_in_reg));
  unsigned short mirr_v = MTABEL_OBIT[reg_nmb].BITFIELD & (((unsigned short)0x8000 >> pos_in_reg));
  if(dir_v != mirr_v){ 
    if(pIO_BIT->LogicType == Direct){
      if(TABEL_OBIT[reg_nmb].BITFIELD & (((unsigned short)0x8000 >> pos_in_reg))){
        TABEL_DPOINT[NPoint].value = 0x01; 
      }else{
       TABEL_DPOINT[NPoint].value = 0x00;
      }
    }else{
      if(TABEL_OBIT[reg_nmb].BITFIELD & (((unsigned short)0x8000 >> pos_in_reg))){
        TABEL_DPOINT[NPoint].value = 0x01; 
      }else{
       TABEL_DPOINT[NPoint].value = 0x00;
      }  
    }
    //init remoove
    TABEL_DPOINT[NPoint].status = STATUS_SETNEW;      
  }  
}
