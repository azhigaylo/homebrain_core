#ifndef _CX_THREADIO
#define _CX_THREADIO

//------------------------------------------------------------------------------
#include "IxRunnable.h"
#include "CxSystemQueue.h"
//------------------------------------------------------------------------------

class CxThreadIO : public IxRunnable, public CxSystemQueue
{   
  enum TThreadIOState
  {
      ST_IO_UNKNOWN = 0,
      ST_IO_DRV_REQUEST,
      ST_IO_WAIT_DRV_RESP,
      ST_IO_NORMAL_WORK
  };

  public:  

     // start all parts of system task
     void Start();

     virtual void CommandProcessor( TCommand &Command ) = 0;
     virtual void ThreadProcessor ( ) = 0;

  protected:    

     // function's   
     CxThreadIO( portCHAR * taskName,  portCHAR * drvName );       
     ~CxThreadIO();
         
     virtual void TaskProcessor();
     
     void DriverIdentificationRequest();
     bool CheckDrvCommand();
     void WorkCycle();
     
     unsigned short threadID;
     unsigned short drvID;     
    
  private:       
     // FSM process
     void Process();
     
     TThreadIOState threadIOState;
     
     char* drvName;
     
     CxThreadIO( const CxThreadIO & );
     CxThreadIO & operator=( const CxThreadIO & );      
     
 }; typedef CxThreadIO *pCxThreadIO;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

