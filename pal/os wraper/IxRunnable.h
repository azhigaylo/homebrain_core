#ifndef _IX_RUNENABLED
#define _IX_RUNENABLED

//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
//------------------------------------------------------------------------------
#define configMAX_TASK_NAME_LEN 15


class IxRunnable
{ 
  public:       

    ~IxRunnable();

    void task_suspend ( );     
    void task_delete  ( );   
    void task_run     ( );

    unsigned long get_sys_tick();
    
  protected:  

    // function's
    IxRunnable( const char * pcName );

    virtual void TaskProcessor();
        
    const char *pcTaskName[ configMAX_TASK_NAME_LEN ];    

	pthread_t *thread;
        
  private:       
    
    void RUN(); 
    
    static void thRunnableFunction( void *pvParameters );
    
    portBASE_TYPE create_thread( );  
    
    IxRunnable( const IxRunnable & );
    IxRunnable & operator=( const IxRunnable & );    
    
}; typedef IxRunnable *pIxRunnable;

//------------------------------------------------------------------------------

#endif // _IX_RUNENABLED

