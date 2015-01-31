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

     ~CxThreadIO();
     // we hide it because everybody should inherit it !  
     CxThreadIO(  const char *taskName,  const char *drvName );   // !!! should be fixed 

     // start all parts of system task
     void Start();

  protected:

     // we hide it because everybody should inherit it !  
     //CxThreadIO(  const char *taskName,  const char *drvName );

     virtual void CommandProcessor( uint16_t ComID, void *data ); //  = 0;
     virtual void ThreadProcessor ( ); // = 0;

     int32_t create_comm_thread( );
     static void *thRunnableCommFunction_ThreadIO( void *args );
     void run_comm();
     void comm_task_delete();

     virtual void TaskProcessor();

     void DriverIdentificationRequest();
     bool CheckDrvCommand();

     uint16_t threadID;
     uint16_t drvID;

     pthread_t commThreadID;

     CxQueue  inQueue;
     CxQueue  outQueue;
     
     uint16_t  initAttempt;	 
     char     pcDrvName[configMAX_DRIVER_NAME_LEN];
     char     pcCommThreadName[configMAX_TASK_NAME_LEN];

  private:
     // FSM process
     void Process();

     TThreadIOState threadIOState;

     CxThreadIO( const CxThreadIO & );
     CxThreadIO & operator=( const CxThreadIO & );

 }; typedef CxThreadIO *pCxThreadIO;
 
//------------------------------------------------------------------------------

#endif // _CX_LAUNCHER

