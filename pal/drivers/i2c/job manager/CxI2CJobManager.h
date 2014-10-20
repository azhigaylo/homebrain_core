#ifndef _CX_I2CJOBBASE
#define _CX_I2CJOBBASE

//------------------------------------------------------------------------------

#include "Utils.h"
#include "CxQueue.h"
#include "CxI2CBusHelper.h"

//------------------------------------------------------------------------------

#define max_job_in_queue  7

#define opType_write      1
#define opType_read       2


//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TI2CJob
{
  unsigned char  status;
  unsigned short driverID;
  unsigned char  nodeAddr;
  unsigned char  opType;
  unsigned char  wrtLength;
  unsigned char  rdLength;
           char  wrtData[10];  
};  
typedef TI2CJob *pTI2CJob;
#pragma pack ( )

//------------------------------------------------------------------------------

class CxI2CJobManager
{  
 public:  
   
   enum TJobStatus
   {
     ST_NOT_INIT = 0,
     ST_READY_FOR_PROCESS,
     ST_IN_PROCESS,
     ST_CALLBACK,
     ST_PROCESSED
   };   
   
   CxI2CJobManager( );
   ~CxI2CJobManager(){}
   
   static CxI2CJobManager & getInstance();

   bool SetJob( TI2CJob &I2CJob );
   void StopAllJob( );
   
   bool WasJobFinished( unsigned short driverID );                              // if "read" operation has been executed - address of block for read.
   pTI2CRecBlock GetJobResult( unsigned short driverID );                       // in other case - NULL
   
 protected:   

   static void callBackFunction( pTI2CRecBlock );

 private:  
   
   bool SetActivJob(); 
   
   void ExecuteActivJob();
   
   void StopActivJob();
   
   // job queue
   CxQueue I2CJobQueue;
   
   // job which is in procession
   TI2CJob activJob;
   
   //helper class
   CxI2CBusHelper I2CBusHelper; 
   // here we will put data which has been recieved from i2c device
   TI2CRecBlock I2CRecBlock;

}; typedef CxI2CJobManager *pCxI2CJobManager;

#endif /*_CX_I2CJOBBASE*/   
