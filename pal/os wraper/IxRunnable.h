#ifndef _IX_RUNENABLED
#define _IX_RUNENABLED

//------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
//------------------------------------------------------------------------------

class IxRunnable
{ 
  public:       

    ~IxRunnable();

    void task_suspend ( );     
    void task_delete  ( );   
    void task_run     ( );
    void task_sleep   ( portTickType xTicksToDelay );
    
    unsigned long get_sys_tick();
    
  protected:  

    // function's   
    IxRunnable( portCHAR * pcName );   

    virtual void TaskProcessor() = 0;
        
    portCHAR  pcTaskName[ configMAX_TASK_NAME_LEN ];    

    xTaskHandle taskdID;        

        
  private:       
    
    void RUN(); 
    
    static void thRunnableFunction( void *pvParameters );
    
    portBASE_TYPE create_thread( );  
    
    IxRunnable( const IxRunnable & );
    IxRunnable & operator=( const IxRunnable & );    
    
}; typedef IxRunnable *pIxRunnable;

//------------------------------------------------------------------------------

#endif // _IX_RUNENABLED

