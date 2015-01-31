#ifndef _IOBit
#define _IOBit

#define  Direct   0x01
#define  Invers   0x02

#define  IDEV     0x01
#define  ODEV     0x02

extern TAPOINT TABEL_APOINT[]; 
extern TDPOINT TABEL_DPOINT[];
extern TBITFIELD TABEL_IBIT[]; 
extern TBITFIELD TABEL_OBIT[]; 

extern TBITFIELD MTABEL_OBIT[]; 
//-----------------------CREATE CLASS FOR External module processing-------------------------
class TIOBIT
{
  public: 
    TIOBIT(pTIO_BIT piIO_BIT, char iRecNumber);                
    ~TIOBIT(){}              
    void IOBITRecProcessor();      
  private:        
    void SetResetIBit(const pTIO_BIT pIO_BIT);   
    void OBitProc(const pTIO_BIT pIO_BIT);    
    char RecNumber;               // total number of record
    pTIO_BIT pIO_BIT;             // pointer on first record unit         
    
                   
}; typedef TIOBIT *pTIOBIT;

#endif /*_IOBit*/

