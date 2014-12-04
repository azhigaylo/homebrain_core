#ifndef _IX_RUNENABLED
#define _IX_RUNENABLED

//------------------------------------------------------------------------------
#include <pthread.h>
#include <time.h>
//------------------------------------------------------------------------------
#include "ptypes.h"
//------------------------------------------------------------------------------
#define configMAX_TASK_NAME_LEN 50
//------------------------------------------------------------------------------

class IxRunnable
{ 
  public:       

    ~IxRunnable();

    void task_delete();   
    void task_run();
	    
  protected:  
    
	// get time in s from thread start
    uint64_t get_time();
	
    virtual void TaskProcessor();
        
    char pcTaskName[configMAX_TASK_NAME_LEN];    

	pthread_t thread;
	time_t    start_time;
        
  private:       
  
    // we hide it because everybody should inherit it !   
    IxRunnable( const char *pcName );
	
    void run(); 
    
    static void * thRunnableFunction( void *args );
    
    int32_t create_thread( );  
    
    IxRunnable( const IxRunnable & ){}
    IxRunnable & operator=( const IxRunnable & ){}
    
}; typedef IxRunnable *pIxRunnable;

//------------------------------------------------------------------------------

#endif // _IX_RUNENABLED

