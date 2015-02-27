#include "kernel.h"

//---------------------------------common list's---------------------------------------------

// common 
TEMOD_List EMOD_List;
                                                                 
//-----------------------CREATE CLASS FOR USO processing-------------------------------------

TEMOD::TEMOD(CUART_BASE_ptr pCOMPORT, unsigned char addr, unsigned short point,
                    unsigned long FileOffset,unsigned char RNumb,unsigned char WNumb)
{
  pCOM          = pCOMPORT;
  Address       = addr;         
  ComErr        = 0;    
  CmdCount      = 0;              
  EMODPointNumb = point;     
  OffsetInFile  = FileOffset; 
  RRecNumb      = RNumb;
  WRecNumb      = WNumb;
  AddToList();
}
 
//
void TEMOD::AddToList()
{
  pNext = NULL;
  if(!EMOD_List.ModuleCounter){
     EMOD_List.FirstModule = EMOD_List.LastModule = this;
  }else{
    EMOD_List.LastModule->pNext = this;
    EMOD_List.LastModule = this;
  }   
  EMOD_List.ModuleCounter++;
}

void TEMOD::AskRegister(unsigned short FReg,unsigned short LReg,pTLINKREC pLINKRECARR)
{
  //if module error request - 1 in 200 cycle   
  if(ComErr >= 6){
    // set error status for USO  
    EMODStatus = USO_Status_NoReply;
    if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus; 
    // set point status in unknown
    for(char rec_n=0;rec_n<RRecNumb;rec_n++){
     char offset= 0;
     for(unsigned short Register=FReg; Register<LReg+1;Register++,offset++){
       if(pLINKRECARR[rec_n].StrtReg==Register){
        switch (pLINKRECARR[rec_n].OpType){
         case WordToApoint  : {TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status = STATUS_UNKNOWN;break;}
         case LongToApoint  : {TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status = STATUS_UNKNOWN;break;}
         case FloatToApoint : {TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status = STATUS_UNKNOWN;break;}
         case HRegToDpoint  : {TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].status = STATUS_UNKNOWN;break;}
         case LRegToDpoint  : {TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].status = STATUS_UNKNOWN;break;}         
         default : break;
        }                        
       } 
     } 
    } // for(char rec_n=0;rec_n<ArrNumb;rec_n++)..                                
    if(ComErr < 20){ComErr++;return;}else ComErr = 7;
  }
  //variable definition    
  struct{    
    char Addr; char Oper_n; char HiReg; char LowReg; char HiNReg;  char LowNReg; unsigned short CRC;                     
  }EMOD_req;    
  // send query on keypad kontroller   
  EMOD_req.Addr = Address; EMOD_req.Oper_n = RdUso;   
  EMOD_req.HiReg = HIGH(FReg); EMOD_req.LowReg = LOW(FReg);   
  EMOD_req.HiNReg  = HIGH(LReg-FReg+2); EMOD_req.LowNReg  = LOW(LReg-FReg+2);    
  EMOD_req.CRC = CRC16_T((unsigned char*)&EMOD_req, sizeof(EMOD_req)-2);      // CRC count       

  // port protection
  if(pCOM->MODE != USO)return;  
  pCOM->RecBufSkip();     // skip old data
  pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&EMOD_req, sizeof(EMOD_req)); 
  pCOM->StartTxD();       // trancmite start 
  char attempt_counter = 0;
  while(attempt_counter<10){
    Sleep(10);
    if(pCOM->IO_UART.cIPack_rec){break;}   
    attempt_counter++;
  }
  if(attempt_counter<10){                
    struct TEMOD_reply{
      char Addr; char Oper_n; char CountByte;unsigned short DataField[500]; 
    }; typedef TEMOD_reply *pTEMOD_reply;
    
    short rec_size = 5+(LReg-FReg+5)*2;
    char* ret_buf = new char[rec_size];memset(ret_buf,0,rec_size);      
    pTEMOD_reply pEMOD_reply = (pTEMOD_reply)ret_buf;

    short n_rec = pCOM->ReadIO_Buf((unsigned char*)pEMOD_reply, rec_size);  
    // CRC analise
    if(CRC16_T((unsigned char*)pEMOD_reply, n_rec)){     
      EMODStatus = USO_Status_COM_Err;
      if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus;        
      ComErr++; 
      delete ret_buf; 
      return;
    }  
    // address analise 
    if(pEMOD_reply->Addr != Address){      
      EMODStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus; 
      ComErr++; 
      delete ret_buf; 
      return;
    }        
    if(pEMOD_reply->CountByte != (LReg-FReg+2)*2){
      EMODStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus; 
      ComErr++; 
      delete ret_buf;       
      return;    
    }    
    // data recieve - OK 
    EMODStatus = USO_Status_OK;
    if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus;  
    //
    for(char reg=0;reg<(LReg-FReg+2);reg++){      
      pEMOD_reply->DataField[reg] = ConvertMBint(pEMOD_reply->DataField[reg]);
    }
   
    for(char rec_n=0;rec_n<RRecNumb;rec_n++){
     char offset= 0;
     for(unsigned short Register=FReg; Register<LReg+1;Register++,offset++){
       if(pLINKRECARR[rec_n].StrtReg==Register){        
        switch (pLINKRECARR[rec_n].OpType){
         case WordToApoint  : {float result = (float)pEMOD_reply->DataField[offset];
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].value = result;
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status  = STATUS_RELIABLE;     
                               break;}
         case LongToApoint  : {float result = (float)(*((long*)(&pEMOD_reply->DataField[offset])));
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].value = result;
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status  = STATUS_RELIABLE;     
                               break;}
         case FloatToApoint : {float result = *((float*)(&pEMOD_reply->DataField[offset])); 
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].value = result;
                               TABEL_APOINT[pLINKRECARR[rec_n].NPoint].status  = STATUS_RELIABLE;     
                               break;}
         case HRegToDpoint  : {TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].value = (char)(HIGH(pEMOD_reply->DataField[offset]));
                               TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].status  = STATUS_RELIABLE;     
                               break;}
         case LRegToDpoint  : {TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].value = (char)(LOW(pEMOD_reply->DataField[offset]));
                               TABEL_DPOINT[pLINKRECARR[rec_n].NPoint].status  = STATUS_RELIABLE;     
                               break;}         
         default : break;
        }
       }  // if(pLINKRECARR[rec_n].StrtReg==Register)..
     }  // for(unsigned short Register=FReg; Register<LReg+1;Register++)..
    } // for(char rec_n=0;rec_n<ArrNumb;rec_n++)..   
    //    
    ComErr = 0;     //clear error flag
    delete ret_buf;
    return;              
  }else{           
    ComErr++;       // if error in communication - int counter error
    return;
  } // if(attempt_counter<7)...     
}

void TEMOD::WrtRegister(pTLINKREC pLINKREC)
{    
  bool needprocess = false;
  //change analise
  if(pLINKREC->OpType == ApointToWord || pLINKREC->OpType == ApointToLong || pLINKREC->OpType == ApointToFloat){
    if(TABEL_APOINT[pLINKREC->NPoint].status == STATUS_SETNEW)needprocess = true;
  }  
  if(pLINKREC->OpType == DpointToReg){
    if(TABEL_DPOINT[pLINKREC->NPoint].status == STATUS_SETNEW)needprocess = true;
  }  
  //write data
  if(needprocess){  
    //if module error request - 1 in 200 cycle   
    if(ComErr >= 6){
      // set error status for USO  
      EMODStatus = USO_Status_NoReply;
      if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus;                             
      if(ComErr > 20)ComErr = 7;
    }    
    // port protection
    if(pCOM->MODE != USO)return;  
    //request structure 
    struct{
      char Addr;char Oper_n;char HiReg; char LowReg; char HiNReg;char LowNReg;char NumbB;
      unsigned short DATA[3];
    }EMOD_wrt; 
   
    // send query on keypad kontroller   
    EMOD_wrt.Addr = Address; EMOD_wrt.Oper_n = WrRegUso;   
    EMOD_wrt.HiReg = HIGH(pLINKREC->StrtReg); EMOD_wrt.LowReg = LOW(pLINKREC->StrtReg);
    EMOD_wrt.HiNReg  = 0;   
    switch (pLINKREC->OpType){
      case ApointToWord  : {EMOD_wrt.LowNReg = 1; 
                            EMOD_wrt.DATA[0] = ConvertMBint((unsigned short)TABEL_APOINT[pLINKREC->NPoint].value);
                            break;
                           }
      case ApointToLong  : {EMOD_wrt.LowNReg = 2; 
                            *((long*)EMOD_wrt.DATA) = (long)TABEL_APOINT[pLINKREC->NPoint].value;
                            EMOD_wrt.DATA[0] = ConvertMBint(EMOD_wrt.DATA[0]);
                            EMOD_wrt.DATA[1] = ConvertMBint(EMOD_wrt.DATA[1]);
                            break;
                           }
      case ApointToFloat : {EMOD_wrt.LowNReg = 2; 
                            *((float*)EMOD_wrt.DATA) = TABEL_APOINT[pLINKREC->NPoint].value;
                            EMOD_wrt.DATA[0] = ConvertMBint(EMOD_wrt.DATA[0]);
                            EMOD_wrt.DATA[1] = ConvertMBint(EMOD_wrt.DATA[1]);      
                            break;
                           }
      case DpointToReg   : {EMOD_wrt.LowNReg = 1; 
                            EMOD_wrt.DATA[0] = ConvertMBint((unsigned short)TABEL_DPOINT[pLINKREC->NPoint].value);
                            break;
                           }     
      default : break;
    }    
    EMOD_wrt.NumbB = EMOD_wrt.LowNReg*2;
    EMOD_wrt.DATA[EMOD_wrt.LowNReg] = CRC16_T((unsigned char*)&EMOD_wrt, 7+EMOD_wrt.NumbB);      // CRC count       
   
    pCOM->RecBufSkip();     // skip old data
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&EMOD_wrt, 9+EMOD_wrt.NumbB); 
    pCOM->StartTxD();       // trancmite start 
    char attempt_counter = 0;
    while(attempt_counter<10){
      Sleep(10);
      if(pCOM->IO_UART.cIPack_rec){break;}   
      attempt_counter++;
    }    
    // link - OK
    if(attempt_counter<10){  
      struct{
        char Addr; char Oper_n; char HiReg; char LowReg; char HiNReg;  char LowNReg; unsigned short CRC;   
      }EMOD_reply;      
      short n_rec = pCOM->ReadIO_Buf((unsigned char*)&EMOD_reply, sizeof(EMOD_reply));  
      // CRC analise
      if(CRC16_T((unsigned char*)&EMOD_reply, n_rec)){     
        EMODStatus = USO_Status_COM_Err;
        if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus;        
        ComErr++; 
        return;
      }  
      // address analise 
      if(EMOD_reply.Addr != Address){      
        EMODStatus = USO_Status_ReplyErr;    // set error status for USO 
        if(EMODPointNumb) TABEL_DPOINT[EMODPointNumb].value = EMODStatus; 
        ComErr++; 
        return;
      }            
      //reset status      
      if(pLINKREC->OpType == ApointToWord || pLINKREC->OpType == ApointToLong || pLINKREC->OpType == ApointToFloat){
        TABEL_APOINT[pLINKREC->NPoint].status = STATUS_PROCESSED;
      }  
      if(pLINKREC->OpType == DpointToReg){
        TABEL_DPOINT[pLINKREC->NPoint].status = STATUS_PROCESSED;
      }
           
     ComErr = 0;     //clear error flag
     CmdCount = 0;   //clear command write attempt
     return;              
    }else{           
     // if error in communication - int counter error      
     if(CmdCount>4){
        if(pLINKREC->OpType == ApointToWord || pLINKREC->OpType == ApointToLong || pLINKREC->OpType == ApointToFloat){
          TABEL_APOINT[pLINKREC->NPoint].status = STATUS_PROCESSED;
        }  
        if(pLINKREC->OpType == DpointToReg){
          TABEL_DPOINT[pLINKREC->NPoint].status = STATUS_PROCESSED;
        } 
        CmdCount=0;      
     }
     ComErr++;       // if error in communication - int counter error
     CmdCount++;     // if error in communication - int attempt counter      
     return;
    } // if(attempt_counter<10)...     
  } // if(needprocess)..
}

// procedure scanning of AI
bool TEMOD::ScanStart() 
{    
  char File[12];memset(File,0,sizeof(File));
  sprintf(File, "ExtMod.lnk");
  //read link-file header
  pTFileHndl iFileHandle = OpenFile(File,f_rd); 
  if(!iFileHandle){return(false);}
  FileSeek(iFileHandle,OffsetInFile,SEEK_SET);    
  if(RRecNumb+WRecNumb == 0){FileClose(iFileHandle);return(false);}
  
  pTLINKREC pLINKRECARR = new TLINKREC[RRecNumb + WRecNumb];
  for(char i=0;i<RRecNumb;i++){
   FileRead(iFileHandle, (char*)(pLINKRECARR+i), sizeof(TLINKREC));
  }
  for(char i=0;i<WRecNumb;i++){
   FileRead(iFileHandle, (char*)(pLINKRECARR+RRecNumb+i), sizeof(TLINKREC));
  }  
  FileClose(iFileHandle); 
    
  //read data algoritm
  unsigned short SectionStrt = pLINKRECARR[0].StrtReg;
  unsigned short CurentStrt = SectionStrt;
  char ZCounter = 1;
  while(true){
    if(ZCounter>=RRecNumb){
      //ack register SectionStrt..pLINKRECARR[ZCounter].StrtReg
      AskRegister(SectionStrt,pLINKRECARR[ZCounter-1].StrtReg,pLINKRECARR);
       break;
    }
    if((pLINKRECARR[ZCounter].StrtReg - CurentStrt)>10){
      //ack register SectionStrt..pLINKRECARR[ZCounter-1].StrtReg
      AskRegister(SectionStrt,pLINKRECARR[ZCounter-1].StrtReg,pLINKRECARR);
      SectionStrt = pLINKRECARR[ZCounter].StrtReg;
    }else{
      if((pLINKRECARR[ZCounter].StrtReg - SectionStrt)>=100){
        //ack register SectionStrt..pLINKRECARR[ZCounter].StrtReg
        AskRegister(SectionStrt,pLINKRECARR[ZCounter].StrtReg,pLINKRECARR);          
        ZCounter++; SectionStrt = pLINKRECARR[ZCounter].StrtReg;
      }
    }
    CurentStrt = pLINKRECARR[ZCounter].StrtReg;
    ZCounter++;
  }// while(true).. 
  
  //write data algoritm
  for(char pos=0;pos<WRecNumb;pos++){
   WrtRegister(pLINKRECARR+(RRecNumb+pos));
  } 
     
  delete[]pLINKRECARR; 
  return(true); 
}


