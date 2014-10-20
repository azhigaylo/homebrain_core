
#include "CxSpiJobManager.h"

//------------------------------------------------------------------------------

CxSpiJobManager::CxSpiJobManager( ):
  I2CJobQueue( max_job_in_queue, sizeof(TI2CJob) )
  ,mSpiBusHelper( callBackFunction )  
{
  mod_memset( reinterpret_cast<char*>(&activJob), 0, sizeof(activJob), sizeof(activJob) );
}

CxSpiJobManager &CxSpiJobManager::getInstance( )
{
  static CxSpiJobManager theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

bool CxSpiJobManager::SetJob( TI2CJob &I2CJob )
{
  bool result = false;
  
  I2CJob.status = ST_READY_FOR_PROCESS;
  result = I2CJobQueue.Send( &I2CJob );

  // try to send job for execution
  if( false == mSpiBusHelper.IsHelperBusy() && ((activJob.status == ST_NOT_INIT) || (activJob.status == ST_PROCESSED)) )
  {
    if( true == SetActivJob() )
    {
      ExecuteActivJob();
    }  
  }  

  return result;
}
  
bool CxSpiJobManager::WasJobFinished( unsigned short driverID )
{
  bool result = false; 
  
  if( driverID == activJob.driverID )
   {
     if( activJob.status == ST_CALLBACK )
     {
       result = true; 
     }  
   }
   return result;
}

pTI2CRecBlock CxSpiJobManager::GetJobResult( unsigned short driverID )
{
  pTI2CRecBlock pI2CRecBlock = NULL;
  
  if( driverID == activJob.driverID )
   {
     if( activJob.status == ST_CALLBACK )
     {
       if( activJob.opType == opType_read )
       {
         pI2CRecBlock = &I2CRecBlock;
       }              
       // activate and put on execution new job
       if( true == SetActivJob() )
       {
         ExecuteActivJob();
       }  
       else
       {
         // stop current activ job
         StopActivJob();
       }      
     }  
   }
   return pI2CRecBlock;
}

void CxSpiJobManager::StopAllJob( )
{

}

//------------------------------------------------------------------------------

bool CxSpiJobManager::SetActivJob( )
{
  bool result = false;
  // get job from queue
  if( I2CJobQueue.Occupancy() > 0 )
  {
    result = I2CJobQueue.Receive( &activJob ); 
  }  
  
  return result;
}

void CxSpiJobManager::ExecuteActivJob( )
{
  // send activJob on execution to CxI2CHelper  
  activJob.status = ST_IN_PROCESS;
  
    switch( activJob.opType )
  {
     case opType_write :
     {
       mSpiBusHelper.WriteMultiple( activJob.nodeAddr, activJob.wrtLength, activJob.wrtData );
       break;
     }  
     case opType_read:
     {
       mSpiBusHelper.ReadMultiple( activJob.nodeAddr, activJob.rdLength );
       break;         
     }         
     default :  {   }    // unknown command     
  }   
  
}

void CxSpiJobManager::StopActivJob( )
{
  activJob.status = ST_PROCESSED;
}

//------------------------------------------------------------------------------

void CxSpiJobManager::callBackFunction( pTI2CRecBlock pRecBlock )
{
  CxSpiJobManager &myInst = CxSpiJobManager::getInstance();
  
  if( myInst.activJob.status == ST_IN_PROCESS )
  {
    myInst.I2CRecBlock = *pRecBlock;
    myInst.activJob.status = ST_CALLBACK;                        
  }  
}
