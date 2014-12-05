#ifndef _CX_THREADIO
#define _CX_THREADIO

//------------------------------------------------------------------------------
#include "ptypes.h"
#include "CxQueue.h"
#include "IxDriver.h"
#include "IxRunnable.h"
//------------------------------------------------------------------------------

class CxThreadIO : public IxRunnable
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
	 
     CxThreadIO(  const char *taskName,  const char *drvName );
     ~CxThreadIO();
	 
  protected:

     // we hide it because everybody should inherit it !  
     CxThreadIO(  const char *taskName,  const char *drvName );

     virtual void TaskProcessor();

     void DriverIdentificationRequest();
     bool CheckDrvCommand();
     void WorkCycle();

     uint16_t threadID;
     uint16_t drvID;

    CxQueue  inQueue;
    CxQueue  outQueue;

     char     pcDrvName[configMAX_DRIVER_NAME_LEN];

  private:
     // FSM process
     void Process();

     TThreadIOState threadIOState;

     CxThreadIO( const CxThreadIO & );
     CxThreadIO & operator=( const CxThreadIO & );

 }; typedef CxThreadIO *pCxThreadIO;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

