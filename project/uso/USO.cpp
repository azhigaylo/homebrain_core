#include "kernel.h"

//---------------------------------common list's---------------------------------------------

// common 
TUso_List USO_List;
// AI uso
TAI_List AI_List; 
                                                                           
//-----------------------CREATE CLASS FOR USO processing-------------------------------------

TUSO_Module::TUSO_Module(CUART_BASE_ptr pCOMPORT, unsigned char addr, unsigned char chan, unsigned short point)
{
  pCOM = pCOMPORT;
  Address    = addr;         
  ComErr     = 0;           
  Channels   = chan;        
  USOPointNumb = point;     
   
  AddToList();
}
 
//
void TUSO_Module::AddToList()
{
  pNext = NULL;
  if(!USO_List.ModuleCounter){
     USO_List.FirstModule = USO_List.LastModule = this;
  }else{
    USO_List.LastModule->pNext = this;
    USO_List.LastModule = this;
  }   
  USO_List.ModuleCounter++;
}

//-----------------------CREATE CLASS FOR AI module processing---------------------------------

TAI_USO::TAI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTAIOChannel ptr,unsigned short USOpoint)
                                                    :TUSO_Module(pPORT, address, chan, USOpoint)
{
  pCh   = ptr; 
  AddToAiList();
}  

//
void TAI_USO::AddToAiList()
{
  pNextAi = NULL;
  if(AI_List.AiModuleCounter == NULL){
    AI_List.FirstModule = AI_List.LastModule = this;
  }else{
    AI_List.LastModule->pNextAi = this;
    AI_List.LastModule = this;
  }   
  AI_List.AiModuleCounter++;
} 

// procedure conversion analog parameter in value
void TAI_USO::ConvAiToVal()
{  
  pTAIOChannel pCurCh = pCh;   
  if(Channels>16) Channels = 16; 
  //chanel processing 
  for(char i=0; i<Channels; i++){        
   switch(pCurCh->Type){
     case ATYPE_AI_20mA   : {pCurCh->MidValue = pCurCh->Code * AI_4_20; break;}   
     case ATYPE_AI_20mA_Z : {pCurCh->MidValue = pCurCh->Code * AI_4_20_Z; break;}   
     case ATYPE_AI_5mA    : {pCurCh->MidValue = pCurCh->Code * AI_0_5; break;}   
     case ATYPE_AI_pm10V  : {pCurCh->MidValue = pCurCh->Code * AI_10_10 - 6.6666; break;}   
     case ATYPE_AI_01V    : {pCurCh->MidValue = pCurCh->Code * AI_0_01; break;}   
     case ATYPE_AI_36V    : {pCurCh->MidValue = pCurCh->Code * AI_0_36; break;}   
     case ATYPE_AI_120V   : {pCurCh->MidValue = pCurCh->Code * AI_0_120; break;}   
     case ATYPE_AI_D300V  : {pCurCh->MidValue = pCurCh->Code * AI_DC_300; break;}   
     case ATYPE_AI_TCM100 : {pCurCh->MidValue = pCurCh->Code * AI_TCP100 + 1.854; break;}  
     case ATYPE_AI_TCP100_A : {pCurCh->MidValue = pCurCh->Code * AI_TCP100_A + 1.854; break;}  
     case ATYPE_AI_A300V  : {pCurCh->MidValue = pCurCh->Code * AI_AC_300; break;}            
     default : {break;}
   }            
                          
   pCurCh->ChanelStatus = A_PARAM_RELIABLE;      //preset status of parameter
   if(pCurCh->MidValue < pCurCh->MinMid) pCurCh->ChanelStatus = A_PARAM_LESSCODESP;     //analise - less code sp     
   if(pCurCh->MidValue > pCurCh->MaxMid) pCurCh->ChanelStatus = A_PARAM_MORECODESP;     //analise - more code sp    
  
   // store value      
   if(pCurCh->MidPointNumb){  
      if((pCurCh->Type != ATYPE_DI3)&&(pCurCh->Type != ATYPE_DI2)&&(pCurCh->Type != ATYPE_DI1)&&
             (pCurCh->Type != ATYPE_DO3)&&(pCurCh->Type != ATYPE_DO2)&&(pCurCh->Type != ATYPE_DO1))
      {
        if(pCurCh->ChanelStatus == A_PARAM_RELIABLE){TABEL_APOINT[pCurCh->MidPointNumb].status = STATUS_RELIABLE;}
         else{TABEL_APOINT[pCurCh->MidPointNumb].status = STATUS_ALARM;}
        TABEL_APOINT[pCurCh->MidPointNumb].value = pCurCh->MidValue;
      }
   }     
   pCurCh++;    
  }  
}

// rocedure conversion analog parameter in parameter
void TAI_USO::ConvAiToParam()
{
  pTAIOChannel pCurCh = pCh;             

  for(char i=0; i<Channels; i++){         
    switch(pCurCh->Type){
      case ATYPE_AI_20mA   : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/(pCurCh->MaxMid - pCurCh->MinMid))*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal;  break;}   
      case ATYPE_AI_20mA_Z : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/(pCurCh->MaxMid - pCurCh->MinMid))*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal;  break;}        
      case ATYPE_AI_5mA    : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/5)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal;   break;}   
      case ATYPE_AI_pm10V  : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/20)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal*2;  break;}   
      case ATYPE_AI_01V    : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/0.1)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal; break;}   
      case ATYPE_AI_36V    : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/36)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal;  break;}   
      case ATYPE_AI_120V   : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/120)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal; break;}   
      case ATYPE_AI_D300V  : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/300)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal; break;}            
      case ATYPE_AI_A300V  : {pCurCh->PhisValue = ((pCurCh->MidValue - pCurCh->MinMid)/300)*(pCurCh->MaxVal - pCurCh->MinVal) + pCurCh->MinVal; break;}            
      case ATYPE_AI_TCM100 : {pCurCh->PhisValue = K - (float)sqrt(K2 + (float)(pCurCh->MidValue/R0-1.0)/B); break;} 
      case ATYPE_AI_TCP100_A : {pCurCh->PhisValue = K - (float)sqrt(K2 + (float)(pCurCh->MidValue/R0-1.0)/B); break;}       
      default   : {break;}
    }        

    pCurCh->ChanelStatus = A_PARAM_RELIABLE;   //preset status of parameter      
    //analise - less code sp
    if(pCurCh->PhisValue < pCurCh->MinVal){pCurCh->ChanelStatus = A_PARAM_LESSVALSP;}  
    //analise - more code sp
    if(pCurCh->PhisValue > pCurCh->MaxVal){pCurCh->ChanelStatus = A_PARAM_MOREVALSP;}                 
    // store value      
    if(pCurCh->ValPointNumb){     
      if((pCurCh->Type != ATYPE_DI3)&&(pCurCh->Type != ATYPE_DI2)&&(pCurCh->Type != ATYPE_DI1)&&
             (pCurCh->Type != ATYPE_DO3)&&(pCurCh->Type != ATYPE_DO2)&&(pCurCh->Type != ATYPE_DO1))
      {
        if(pCurCh->ChanelStatus == A_PARAM_RELIABLE){TABEL_APOINT[pCurCh->ValPointNumb].status = STATUS_RELIABLE;}
         else{TABEL_APOINT[pCurCh->ValPointNumb].status = STATUS_ALARM;}
        TABEL_APOINT[pCurCh->ValPointNumb].value = pCurCh->PhisValue;
      }else{
        if(pCurCh->Type == ATYPE_DI3){
          if((pCurCh->Code) & 0x0001)TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x00;
          if((pCurCh->Code) & 0x0002)TABEL_DPOINT[pCurCh->ValPointNumb+1].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+1].value = 0x00;
          if((pCurCh->Code) & 0x0004)TABEL_DPOINT[pCurCh->ValPointNumb+2].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+2].value = 0x00;
          TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_RELIABLE; 
          TABEL_DPOINT[pCurCh->ValPointNumb+1].status = STATUS_RELIABLE; 
          TABEL_DPOINT[pCurCh->ValPointNumb+2].status = STATUS_RELIABLE;          
        }
        if(pCurCh->Type == ATYPE_DI2){
          if((pCurCh->Code) & 0x0001)TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x00;
          if((pCurCh->Code) & 0x0002)TABEL_DPOINT[pCurCh->ValPointNumb+1].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+1].value = 0x00;      
          TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_RELIABLE; 
          TABEL_DPOINT[pCurCh->ValPointNumb+1].status = STATUS_RELIABLE;           
        }    
        if(pCurCh->Type == ATYPE_DI1){
          if((pCurCh->Code) & 0x0001)TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x01; else TABEL_DPOINT[pCurCh->ValPointNumb+0].value = 0x00;
          TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_RELIABLE; 
        }             
      }
    }
    pCurCh++;                  
  } // for(char i=0; i<Channels; i++)
}

// procedure scanning of AI
bool TAI_USO::ScanAI() 
{    
  //variable definition 
  unsigned char CurAskAddrUSO = Address;      // address of current interragation USO module
  //if module error request - 1 in 200 cycle   
  if(ComErr >= 4){
    // set error status for USO  
    USOStatus = USO_Status_NoReply;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    // set error status for chanel 
    pTAIOChannel pCurCh = pCh;
    for(char i=0; i<Channels; i++){                         
      // set status      
      if(pCurCh->ValPointNumb){     
         if((pCurCh->Type != ATYPE_DI3)&&(pCurCh->Type != ATYPE_DI2)&&(pCurCh->Type != ATYPE_DI1)&&
                (pCurCh->Type != ATYPE_DO3)&&(pCurCh->Type != ATYPE_DO2)&&(pCurCh->Type != ATYPE_DO1))
          TABEL_APOINT[pCurCh->ValPointNumb].status = STATUS_UNKNOWN;
         else{           
           if(pCurCh->Type == ATYPE_DI3){
             TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_UNKNOWN; 
             TABEL_DPOINT[pCurCh->ValPointNumb+1].status = STATUS_UNKNOWN; 
             TABEL_DPOINT[pCurCh->ValPointNumb+2].status = STATUS_UNKNOWN;          
           }
           if(pCurCh->Type == ATYPE_DI2){
             TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_UNKNOWN; 
             TABEL_DPOINT[pCurCh->ValPointNumb+1].status = STATUS_UNKNOWN;           
           }    
           if(pCurCh->Type == ATYPE_DI1){           
             TABEL_DPOINT[pCurCh->ValPointNumb+0].status = STATUS_UNKNOWN; 
           }                        
         }                  
      } // if(pCurCh->ValPointNumb)..
      if(pCurCh->MidPointNumb){     
        if((pCurCh->Type != ATYPE_DI3)&&(pCurCh->Type != ATYPE_DI2)&&(pCurCh->Type != ATYPE_DI1)&&
                (pCurCh->Type != ATYPE_DO3)&&(pCurCh->Type != ATYPE_DO2)&&(pCurCh->Type != ATYPE_DO1))        
        TABEL_APOINT[pCurCh->MidPointNumb].status = STATUS_UNKNOWN;
      }                  
      pCurCh++;                  
    }            
    if(ComErr < 20){ComErr++;return(false);}else ComErr = 5;
  }
     
  struct{    
    char Addr; char Oper_n;
    char HiStartReg; char LoStartReg;      
    char HiNumbReg;  char LoNumbReg;
    unsigned short CRC;                     
  }Ai_request; 
   
  // send query on keypad kontroller   
  Ai_request.Addr       = CurAskAddrUSO;
  Ai_request.Oper_n     = RdUso;   
  Ai_request.HiStartReg = 0; 
  Ai_request.LoStartReg = 0;   
  Ai_request.HiNumbReg  = 0;   
  Ai_request.LoNumbReg  = Channels;     
  Ai_request.CRC = CRC16_T((unsigned char*)&Ai_request, sizeof(Ai_request)-2);      // CRC count     
  
  // port protection
  if(pCOM->MODE != USO)return(false);
  
  pCOM->RecBufSkip();     // skip old data
  pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&Ai_request, sizeof(Ai_request)); 
  pCOM->StartTxD();       // trancmite start 
  char attempt_counter = 0;
  while(attempt_counter<10){
    Sleep(10);
    if(pCOM->IO_UART.cIPack_rec){break;}   
    attempt_counter++;
  }
  if(attempt_counter<10){                
    pTAIOChannel pCurCh = pCh;
    struct TAi_reply{
      char Addr; char Oper_n; char CountByte;        
      unsigned short DataField[16]; 
      unsigned short CRC;
    }; typedef TAi_reply *pTAi_reply;
    
    pTAi_reply pAi_reply = new TAi_reply;

    short n_rec = pCOM->ReadIO_Buf((unsigned char*)pAi_reply, sizeof(TAi_reply));       
    // CRC analise
    if(CRC16_T((unsigned char*)pAi_reply, n_rec)){     
      USOStatus = USO_Status_COM_Err;     // set error status for USO 
      if(USOPointNumb)TABEL_DPOINT[USOPointNumb].value = USOStatus; 
      ComErr++; 
      delete pAi_reply; 
      return(false);
    }
    // address analise 
    if(pAi_reply->Addr != CurAskAddrUSO){      
      USOStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(USOPointNumb)TABEL_DPOINT[USOPointNumb].value = USOStatus;
      ComErr++; 
      delete pAi_reply; 
      return(false);
    } 
    if(pAi_reply->CountByte != Channels*2){
      USOStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus; 
      ComErr++; 
      delete pAi_reply;       
      return(false);    
    }     
    // data recieve - OK 
    USOStatus = USO_Status_OK;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    // copy chanel data          
    for(char i=0; i<Channels; i++,pCurCh++){
      if((pCurCh->Type==ATYPE_AO_Gen)||(pCurCh->Type==ATYPE_DO3)||
                  (pCurCh->Type==ATYPE_DO2)||(pCurCh->Type==ATYPE_DO1)) continue;         
      pCurCh->Code = ConvertMBint(pAi_reply->DataField[i]);            
    }    
        
    ComErr = 0;     //clear error flag
    delete pAi_reply;
    return(true);    
  }else{           
    ComErr++;       // if error in communication - int counter error
    return(false);
  }   
}
       
// setting procedure for AO/DO
void TAI_USO::SetAO() 
{          
  // port protection
  if(pCOM->MODE != USO)return;
  // set address of current interragation USO module    
  struct{ 
    char Addr; char Oper_n; char HiNumbCh; char LoNumbCh; char HiValCh; char LoValCh;
    unsigned short CRC;                       
  }Ai_request; 
  Ai_request.Addr = Address; Ai_request.Oper_n  = WrUso;     
  Ai_request.HiNumbCh = 0;   Ai_request.HiValCh = 0;       

  pTAIOChannel pCurCh = pCh;
  for(char Cur_Chan=0; Cur_Chan<Channels; Cur_Chan++,pCurCh++){                         
   bool needrewrite = false; unsigned short NewValue = 0;
   if(pCurCh->ValPointNumb){
     if(pCurCh->Type==ATYPE_AO_Gen){     
      if(TABEL_APOINT[pCurCh->ValPointNumb].status == STATUS_SETNEW){
        NewValue = (unsigned short)(TABEL_APOINT[pCurCh->ValPointNumb].value);
        needrewrite = true;
      }
     }     
     if(pCurCh->Type==ATYPE_DO3){     
      if((TABEL_DPOINT[pCurCh->ValPointNumb].status == STATUS_SETNEW) || 
         (TABEL_DPOINT[(pCurCh->ValPointNumb)+1].status == STATUS_SETNEW) ||      
         (TABEL_DPOINT[(pCurCh->ValPointNumb)+2].status == STATUS_SETNEW)){        
        if(TABEL_DPOINT[pCurCh->ValPointNumb].value) NewValue |= 0x01;
        if(TABEL_DPOINT[(pCurCh->ValPointNumb)+1].value) NewValue |= 0x02;
        if(TABEL_DPOINT[(pCurCh->ValPointNumb)+2].value) NewValue |= 0x04;        
        needrewrite = true;
      } 
     } // if(pCurCh->Type==ATYPE_DO3)
     if(pCurCh->Type==ATYPE_DO2){     
      if((TABEL_DPOINT[pCurCh->ValPointNumb].status == STATUS_SETNEW) || 
         (TABEL_DPOINT[(pCurCh->ValPointNumb)+1].status == STATUS_SETNEW)){        
        if(TABEL_DPOINT[pCurCh->ValPointNumb].value) NewValue |= 0x01;
        if(TABEL_DPOINT[(pCurCh->ValPointNumb)+1].value) NewValue |= 0x02;       
        needrewrite = true;
      } 
     } // if(pCurCh->Type==ATYPE_DO2)..
     if(pCurCh->Type==ATYPE_DO1){     
      if(TABEL_DPOINT[pCurCh->ValPointNumb].status == STATUS_SETNEW){        
        if(TABEL_DPOINT[pCurCh->ValPointNumb].value) NewValue |= 0x01;     
        needrewrite = true;
      } 
     } // if(pCurCh->Type==ATYPE_DO1)..     
   }//if(pCurCh->ValPointNumb)   
   
   // send query on keypad kontroller   
   if(needrewrite){
     //NewValue = ConvertMBint(NewValue);
     Ai_request.HiValCh   = HIGH(NewValue);      
     Ai_request.LoValCh   = LOW(NewValue);          
     Ai_request.LoNumbCh  = Cur_Chan;                          
     Ai_request.CRC = CRC16_T((unsigned char*)&Ai_request, sizeof(Ai_request)-2);      // CRC count     
    
     pCOM->RecBufSkip();     // skip old data
     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&Ai_request, sizeof(Ai_request)); 
     pCOM->StartTxD();       // trancmite start 
     char attempt_counter = 0;
     while(attempt_counter<10){
      Sleep(10);
      if(pCOM->IO_UART.cIPack_rec){break;}   
      attempt_counter++;
     }                  
     // if ret response   
     if(attempt_counter<10){                
      struct{
        char Addr; char Oper_n; char HiReg; char LowReg; char HiNReg;  char LowNReg; unsigned short CRC;   
      }EMOD_reply;      
      short n_rec = pCOM->ReadIO_Buf((unsigned char*)&EMOD_reply, sizeof(EMOD_reply));        
      if(CRC16_T((unsigned char*)&EMOD_reply, n_rec)){continue;} // CRC analise       
      if(EMOD_reply.Addr != Address){continue;}  // address analise 
      
      //reset STATUS_SETNEW status
      if(pCurCh->Type==ATYPE_AO_Gen){TABEL_APOINT[pCurCh->ValPointNumb].status = STATUS_PROCESSED;}
      if(pCurCh->Type==ATYPE_DO3){
        TABEL_DPOINT[pCurCh->ValPointNumb].status = STATUS_PROCESSED;
        TABEL_DPOINT[(pCurCh->ValPointNumb)+1].status = STATUS_PROCESSED;
        TABEL_DPOINT[(pCurCh->ValPointNumb)+2].status = STATUS_PROCESSED;      
      }
      if(pCurCh->Type==ATYPE_DO2){
        TABEL_DPOINT[pCurCh->ValPointNumb].status = STATUS_PROCESSED;
        TABEL_DPOINT[(pCurCh->ValPointNumb)+1].status = STATUS_PROCESSED;
      }
      if(pCurCh->Type==ATYPE_DO1){
        TABEL_DPOINT[pCurCh->ValPointNumb].status = STATUS_PROCESSED;
      }                          
      ComErr = 0;     //clear error flag
     } //if(attempt_counter<10) 
    } //if(needrewrite)..                    
  } //for(char Cur_Chan=0; ..       
  return; 
} 

//-----------------------CREATE CLASS FOR MUK module processing--------------------------------

TMUK_USO::TMUK_USO(CUART_BASE_ptr pPORT,unsigned char address,unsigned short USOpoint,unsigned short InfoBlkPoint,unsigned short ComBlkPoint):TUSO_Module(pPORT, address, 1, USOpoint)
{
   ComBlcPointNumb = ComBlkPoint;   // set start point of command block
   InfoBlcPointNumb = InfoBlkPoint; // set start point of information block
   LastCommand  = 0;            // command for MUK
   CmdCount     = 0;            // command attempt 
   MukMustState = 0;            // state in which MUK must be state
   CLEAR_VALVE(&V);
}


// calculate TAP propertis
void TMUK_USO::CLEAR_VALVE(TVALVE *pV)
{
  pV->MUK = 0; pV->Valve = 0; pV->Gear = 0;
}

// calculate TAP propertis
void TMUK_USO::CalcVALVE(char Data[5])
{
 char Data0 = Data[0] & 0xE3;
 char Data1 = Data[1] & 0x1E;
 char Data2 = Data[2] & 0x1F;
 char Data3 = Data[3] & 0x1F;
 TVALVE v; CLEAR_VALVE(&v);
  //---  ---
  char b = ((Data0&3)<<1) | (Data2&1);
  if (b&4) {v.MUK=MUK_SleepMode; goto Done;}
  if (b==2) {v.MUK=MUK_LocalMode; goto Calc;} 
  if (b!=1) goto Done; 
  v.MUK=MUK_OK;
Calc: 
  b = (Data0&0xA0) | (Data1&8) | ((Data2&8)>>1);
  if ((Data[1]&0x50)==0x40) b|=0x80;
  if ((Data[2]&0x50)==0x40) b|=0x20;
  if (b>=0xA0) {v.MUK=MUK_Fail; goto Done;} 
  switch (b){
    case 0: v.Valve=VALVE_Middle; break;
    case 8: case 0x2C: case 0x28: v.Valve=VALVE_Open; break;
    case 4: case 0x8C: case 0x84: v.Valve=VALVE_Close; break;
    case 0x88: case 0x80: v.Valve=VALVE_Opening; break;
    case 0x24: case 0x20: v.Valve=VALVE_Closing; break;
    //VALVE_Unknown
  }
  //---  ---
  v.Gear = GEAR_OK;
  // 
  if (b&0x80) v.Gear|=GEAR_Opening;
  if (b&0x20) v.Gear|=GEAR_Closing;
  // 
  if (Data3&8) v.Gear|=GEAR_Tightening;
  //
  if ((Data1|(Data2&0x1E)|Data3)&0x17) { // 
    v.Gear &= ~GEAR_OK;
    if (Data3&0x10) v.Gear|=GEAR_Leakage;
    // 
    if (Data1&4) v.Gear|=GEAR_ShortO;
    if (Data2&4) v.Gear|=GEAR_ShortC;
    if (Data3&4) v.Gear|=GEAR_ShortT;
    // 
    if (Data3&2) v.Gear|=GEAR_BreakT;
    if (Data1&2) { v.Gear|=GEAR_BreakO; if (Data1&0x10) { v.Gear|=GEAR_ShortO; Data1&=~0x10; } }
    if (Data2&2) { v.Gear|=GEAR_BreakC; if (Data2&0x10) { v.Gear|=GEAR_ShortC; Data2&=~0x10; } }
    if (Data3&1) {
      b = (Data1&0x10) | ((Data2&0x10)>>1);
      if (b==0) v.Gear|=GEAR_SelfChgM; 
      if (b==0x10) v.Gear|=GEAR_SelfChgO;
      if (b==8) v.Gear|=GEAR_SelfChgC; 
    } else {
      if (Data1&0x10) { v.Gear|=GEAR_OpenJamming; }
      if (Data2&0x10) { v.Gear|=GEAR_CloseJamming;}
    }
  }
  if (v.Gear&(GEAR_ShortO|GEAR_ShortC|GEAR_BreakO|GEAR_BreakC)
      && v.Valve==VALVE_Middle) v.Valve=VALVE_Unknown;
Done:
  V = v;
} 

void TMUK_USO::MukAnalise()
{
   if(InfoBlcPointNumb){
     TABEL_DPOINT[InfoBlcPointNumb].status = V.MUK; 
     TABEL_DPOINT[InfoBlcPointNumb].value  = V.Valve;
     *((unsigned short*)(TABEL_DPOINT + InfoBlcPointNumb + 1)) = V.Gear;
   }  
} 
 
//----------procedure scanning of AI--------------------------
bool TMUK_USO::ScanMUK() 
{      
  unsigned char CurAskAddrUSO = Address;         // address of current interragation USO module
  //if module error request - 1 in 60 cycle
  if(ComErr >= 4){
    // set error status for USO  
    USOStatus = USO_Status_NoReply;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    if(InfoBlcPointNumb){
      TABEL_DPOINT[InfoBlcPointNumb].status = 0; 
      TABEL_DPOINT[InfoBlcPointNumb].value  = 0;
      *((unsigned short*)(TABEL_DPOINT + InfoBlcPointNumb + 1)) = 0;
    } 
    if(ComErr < 20){ComErr++;return(false);}else ComErr = 5;
  }
       
   struct{    
      char Addr; char Oper_n;
      char HiStartReg; char LoStartReg;      
      char HiNumbReg;  char LoNumbReg;
      unsigned short CRC;                     
    }Muk_request; 
   
   Muk_request.Addr        = CurAskAddrUSO;
   Muk_request.Oper_n      = RdUso; 
   Muk_request.HiStartReg = 0; Muk_request.LoStartReg = 0;      
   Muk_request.HiNumbReg  = 0; Muk_request.LoNumbReg  = 0;
   Muk_request.CRC = CRC16_T((unsigned char*)&Muk_request, sizeof(Muk_request)-2);      
      
   // port protection
   if(pCOM->MODE != USO)return(false);
     
   pCOM->RecBufSkip();     // skip old data
   pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&Muk_request, sizeof(Muk_request)); 
   pCOM->StartTxD();       // trancmite start 
   char attempt_counter = 0;
   while(attempt_counter<10){
    Sleep(10);
    if(pCOM->IO_UART.cIPack_rec){break;}
    attempt_counter++;
   }
   if(attempt_counter<10){                
     struct TMuk_reply{ 
       char Addr; char Oper_n; char CountByte; 
       char Reserv; char Data[5]; unsigned short CRC; 
     }Muk_reply;

     short n_rec = pCOM->ReadIO_Buf((unsigned char*)&Muk_reply, sizeof(TMuk_reply));            
     // CRC analise   
     if(CRC16_T((unsigned char*)&Muk_reply, n_rec)){
       USOStatus = USO_Status_COM_Err;     // set error status for USO 
       if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus; 
       ComErr++; 
       return(false);
     }    
     // address analise 
     if(Muk_reply.Addr != CurAskAddrUSO){
       USOStatus = USO_Status_ReplyErr;    // set error status for USO 
       if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;       
       ComErr++;        
       return(false);
     } 
     // data recieve - OK 
     USOStatus = USO_Status_OK;
     if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;      
     // data recieve - OK  
     CalcVALVE(Muk_reply.Data);               
     //clear error flag
     ComErr = 0;    
     return(true);    
    }else{
      // if error in communication - int counter error     
      ComErr++;      
      return(false);
    }                
}

void TMUK_USO::SetMUK() 
{ 
  unsigned char CurAskAddrUSO = Address;         // address of current interragation USO module
  // search command 
  if(ComBlcPointNumb){
    if(TABEL_DPOINT[ComBlcPointNumb].value){
      LastCommand = TABEL_DPOINT[ComBlcPointNumb].value;
      TABEL_DPOINT[ComBlcPointNumb].value = 0;
      TABEL_DPOINT[ComBlcPointNumb].status = STATUS_PROCESSED;
    }      
  }
  // command analise
  if(LastCommand){
    //if module error request - 1 in 60 cycle
    if(ComErr >= 4){
      if(ComErr < 20){ComErr++;return;}else ComErr = 5;
    }
       
    struct{    
      char Addr; char Oper_n;
      char HiStartReg; char LoStartReg;      
      char HiNumbReg;  char LoNumbReg;
      unsigned short CRC;                     
    }Muk_request; 
   
    Muk_request.Addr        = CurAskAddrUSO;
    Muk_request.Oper_n      = WrUso; 
    Muk_request.HiStartReg = 0; Muk_request.LoStartReg = LastCommand;      
    Muk_request.HiNumbReg  = 0; Muk_request.LoNumbReg  = 1;        
    Muk_request.CRC = CRC16_T((unsigned char*)&Muk_request, sizeof(Muk_request)-2);      
      
    // port protection
    if(pCOM->MODE != USO)return;
     
    pCOM->RecBufSkip();     // skip old data
    pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&Muk_request, sizeof(Muk_request)); 
    pCOM->StartTxD();       // trancmite start 
    char attempt_counter = 0;
    while(attempt_counter<10){
     Sleep(10);
     if(pCOM->IO_UART.cIPack_rec){break;}
     attempt_counter++;
    } 
    if(attempt_counter<10){
      struct TMuk_reply{    
        char Addr; char Oper_n;
        char HiStartReg; char LoStartReg; char HiNumbReg;  char LoNumbReg;
        unsigned short CRC;                     
      }Muk_reply; 
                             
      short n_rec = pCOM->ReadIO_Buf((unsigned char*)&Muk_reply, sizeof(TMuk_reply));            
      // CRC analise 
      if(CRC16_T((unsigned char*)&Muk_reply, n_rec)){
        if(CmdCount>4){CmdCount=0; LastCommand = 0;return;}     //clear command 
        CmdCount++;        
        ComErr++; 
        return;
      }      
      // address analise 
      if(Muk_reply.Addr != CurAskAddrUSO){
        if(CmdCount>4){CmdCount=0; LastCommand = 0;return;}     //clear command 
        CmdCount++;        
        ComErr++; 
        return;
      } 
      // data recieve - OK                   
      MukMustState = LastCommand;
      LastCommand = 0;     //clear command  
      CmdCount = 0;        //clear command write attempt
      ComErr = 0;          //clear error flag        
      return;    
    }else{
       // if error in communication - int counter error      
       if(CmdCount>4){CmdCount=0; LastCommand = 0;return;}     //clear command 
       CmdCount++;
       ComErr++;      
       return;
    }   
  } // end - if(LastCommand)     
}
 
//-----------------------CREATE CLASS FOR OBLIK PROCESSOR--------------------------------
TLO1111_USO::TLO1111_USO(CUART_BASE_ptr pPORT,char address,unsigned short USOpoint,unsigned short PPointNumb,unsigned short SPointNumb):TUSO_Module(pPORT, address, 1, USOpoint)
{
  PowerPointNumb = PPointNumb;       // point for power value save
  StealPointNumb = SPointNumb;       // point for steal time save
}

bool TLO1111_USO::ScanLO()
{
  //if module error request - 1 in 60 cycle
  if(ComErr >= 4){
    // set error status for USO  
    USOStatus = USO_Status_NoReply;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    if(PowerPointNumb){TABEL_APOINT[PowerPointNumb].status = STATUS_UNKNOWN;} 
    if(StealPointNumb){TABEL_APOINT[StealPointNumb].status = STATUS_UNKNOWN;}     
    if(ComErr < 20){ComErr++;return(false);}else ComErr = 5;
  }
       
   struct{    
      char key_1; char key_2; char Addr; char TtlLength;      
      char Segment; char User; unsigned short Disp; char DataLength;   
      char XORI;                     
    }LO_request; 
   
   LO_request.key_1 = 0xA5;    LO_request.key_2 = 0x5A; 
   LO_request.Addr  = Address; LO_request.TtlLength  = 0x08;
   LO_request.Segment = 0x20;  LO_request.User = 0x00;
   LO_request.Disp  = 0x00;    LO_request.DataLength = 0x0A;
   LO_request.XORI = LO_request.Addr ^ LO_request.TtlLength ^ LO_request.Segment ^ LO_request.User ^
   LO_request.Disp ^ LO_request.DataLength;

   // port protection
   if(pCOM->MODE != USO)return(false);
     
   pCOM->RecBufSkip();     // skip old data
   pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&LO_request, sizeof(LO_request)); 
   pCOM->StartTxD();       // trancmite start 
 
   char attempt_counter = 0;
   while(attempt_counter<10){
    Sleep(20);
    if(pCOM->IO_UART.cIPack_rec){break;}
    attempt_counter++;
   }
   if(attempt_counter<10){                
     struct TLO_reply{    
       char Result1; char TtlLength; char Result2; char DataLength;  
       unsigned long en_hundreths; unsigned long theft_time;
       char null_energy_count;     char null_time_count;
       char Data[10];       
     }LO_reply;     

     short n_rec = pCOM->ReadIO_Buf((unsigned char*)&LO_reply, sizeof(TLO_reply));             
     // address analise 
     if(LO_reply.Result1 != 0x01){
       USOStatus = USO_Status_ReplyErr;    // set error status for USO 
       if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;       
       ComErr++;        
       return(false);
     } 
     // data recieve - OK 
     USOStatus = USO_Status_OK;
     if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;      
     // data recieve - OK                   
     if(PowerPointNumb){
       TABEL_APOINT[PowerPointNumb].status = A_PARAM_RELIABLE;
       TABEL_APOINT[PowerPointNumb].value =  ((float)OblikLongConvert(LO_reply.en_hundreths))/100;
     } 
     if(StealPointNumb){
       TABEL_APOINT[StealPointNumb].status  = A_PARAM_RELIABLE;
       TABEL_APOINT[StealPointNumb].value  = ((float)OblikLongConvert(LO_reply.theft_time))/100;   
     }   
     //clear error flag
     ComErr = 0;    
     return(true);    
   }else{
      // if error in communication - int counter error     
      ComErr++;      
      return(false);
   }                
}


//-----------------------CREATE CLASS FOR AI module processing---------------------------------
TDI_USO::TDI_USO(CUART_BASE_ptr pPORT, unsigned char address, unsigned char chan, pTDIOChannel ptr,unsigned short USOpoint)
                                                    :TUSO_Module(pPORT, address, chan, USOpoint)
{
  pCh = ptr; 
}  

// procedure scanning of DI
void TDI_USO::ScanDI() 
{  
  //if module error request - 1 in 200 cycle   
  if(ComErr >= 4){
    // set error status for USO  
    USOStatus = USO_Status_NoReply;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    // set error status for chanel 
    pTDIOChannel pCurCh = pCh;
    for(char i=0; i<Channels; i++){                              
      if(pCurCh->PointNumb){TABEL_DPOINT[pCurCh->PointNumb].status = STATUS_UNKNOWN;}                 
      pCurCh++;                  
    }            
    if(ComErr < 20){ComErr++;return;}else ComErr = 5;
  }
     
  struct{    
    char Addr; char Oper_n;
    char HiStartReg; char LoStartReg;char HiNumbReg;  char LoNumbReg;unsigned short CRC;                     
  }Di_request;    
  // send query on keypad kontroller   
  Di_request.Addr       = Address;
  Di_request.Oper_n     = RdUso;   
  Di_request.HiStartReg = 0; 
  Di_request.LoStartReg = 0;   
  Di_request.HiNumbReg  = 0;   
  Di_request.LoNumbReg  = 0x01;     
  Di_request.CRC = CRC16_T((unsigned char*)&Di_request, sizeof(Di_request)-2);      // CRC count       
  // port protection
  if(pCOM->MODE != USO)return;
  
  pCOM->RecBufSkip();     // skip old data
  pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&Di_request, sizeof(Di_request)); 
  pCOM->StartTxD();       // trancmite start   
  char attempt_counter = 0;
  while(attempt_counter<10){
    Sleep(10);
    if(pCOM->IO_UART.cIPack_rec){break;}   
    attempt_counter++;
  }
  //
  if(attempt_counter<10){                
    struct TDi_reply{
      char Addr; char Oper_n; char CountByte;        
      unsigned short DataField[1]; 
      unsigned short CRC;
    }; typedef TDi_reply *pTDi_reply;
    
    pTDi_reply pDi_reply = new TDi_reply;

    short n_rec = pCOM->ReadIO_Buf((unsigned char*)pDi_reply, sizeof(TDi_reply));       
    // CRC analise
    if(CRC16_T((unsigned char*)pDi_reply, n_rec)){     
      USOStatus = USO_Status_COM_Err;     // set error status for USO 
      if(USOPointNumb)TABEL_DPOINT[USOPointNumb].value = USOStatus; 
      ComErr++; 
      delete pDi_reply; 
      return;
    }
    // address analise 
    if(pDi_reply->Addr != Address){      
      USOStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(USOPointNumb)TABEL_DPOINT[USOPointNumb].value = USOStatus;
      ComErr++; 
      delete pDi_reply; 
      return;
    } 
    if(pDi_reply->CountByte != 2){
      USOStatus = USO_Status_ReplyErr;    // set error status for USO 
      if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus; 
      ComErr++; 
      delete pDi_reply;       
      return;    
    }     
    // data recieve - OK 
    USOStatus = USO_Status_OK;
    if(USOPointNumb) TABEL_DPOINT[USOPointNumb].value = USOStatus;    
    // copy chanel data          
    pTDIOChannel pCurCh = pCh;
    unsigned short datafield = ConvertMBint(pDi_reply->DataField[0]); 
    for(char i=0; i<Channels; i++,pCurCh++){      
      if((pCurCh->PointNumb)&&(pCurCh->Type == ATYPE_DI1)){
        TABEL_DPOINT[pCurCh->PointNumb].status = STATUS_RELIABLE;  
        if(datafield & (0x8000>>i))TABEL_DPOINT[pCurCh->PointNumb].value = true;
         else TABEL_DPOINT[pCurCh->PointNumb].value = false;           
      }         
    }       
    ComErr = 0;     //clear error flag
    delete pDi_reply;
    return;    
  }else{           
    ComErr++;       // if error in communication - int counter error
    return;
  }    
}
       
// setting procedure for AO/DO
void TDI_USO::SetDO() 
{          
  // port protection
  if(pCOM->MODE != USO)return;  
  // set address of current interragation USO module    
  struct{
   char Addr;char Oper_n;char HiReg; char LowReg; char HiNReg;char LowNReg;char NumbB;
   unsigned short DATA; unsigned short CRC;
  }DO_request;  
  DO_request.Addr = Address; DO_request.Oper_n  = WrRegUso; 
  DO_request.HiReg = 0; DO_request.LowReg = 0;     
  DO_request.HiNReg = 0; DO_request.LowNReg = 1;  

  unsigned short NewDataValue = 0; char DoChanelCntr = 0;
  pTDIOChannel pCurCh = pCh;
  bool needrewrite = false;   
  for(char Cur_Chan=0; Cur_Chan<Channels; Cur_Chan++,pCurCh++){                         
   if(pCurCh->Type==ATYPE_DO1){      
     if(pCurCh->PointNumb){           
      if(TABEL_DPOINT[pCurCh->PointNumb].status == STATUS_SETNEW)needrewrite = true;
      if(TABEL_DPOINT[pCurCh->PointNumb].value)NewDataValue |= (0x8000>>DoChanelCntr);      
     }    
     DoChanelCntr++; 
   }
  }      
  // send query on keypad kontroller   
  if(needrewrite){
     NewDataValue = ConvertMBint(NewDataValue);
     DO_request.DATA = NewDataValue;                                        
     DO_request.NumbB = DO_request.LowNReg*2;     
     DO_request.CRC = CRC16_T((unsigned char*)&DO_request, sizeof(DO_request)-2); 
    
     pCOM->RecBufSkip();     // skip old data
     pCOM->StopTxD(); pCOM->LoadIO_Buf((unsigned char*)&DO_request, sizeof(DO_request)); 
     pCOM->StartTxD();       // trancmite start 
     char attempt_counter = 0;
     while(attempt_counter<10){
      Sleep(10);
      if(pCOM->IO_UART.cIPack_rec){break;}   
      attempt_counter++;
     }   
     // if ret response   
     if(attempt_counter<10){                
      struct{
        char Addr; char Oper_n; char HiReg; char LowReg; char HiNReg;  char LowNReg; unsigned short CRC;   
      }USO_reply;      
      short n_rec = pCOM->ReadIO_Buf((unsigned char*)&USO_reply, sizeof(USO_reply));        
      if(CRC16_T((unsigned char*)&USO_reply, n_rec)){return;} // CRC analise       
      if(USO_reply.Addr != Address){return;}  // address analise    
      //reset STATUS_SETNEW status
      pCurCh = pCh;  
      for(char Cur_Chan=0; Cur_Chan<Channels; Cur_Chan++,pCurCh++){                         
       if(pCurCh->Type==ATYPE_DO1){      
         if(pCurCh->PointNumb)TABEL_DPOINT[pCurCh->PointNumb].status = STATUS_PROCESSED;               
       }    
      }                         
      ComErr = 0;     //clear error flag
     } //if(attempt_counter<10) 
  } //if(needrewrite)..                          
  return;
} 
