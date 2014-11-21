#ifndef _IX_DRIVER
#define _IX_DRIVER

//------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "CxSystemQueue.h"
//------------------------------------------------------------------------------

// driver idettification request format

class IxDriver :  public CxSystemQueue
{ 
  public:       

    ~IxDriver();

    void task_suspend ( );     
    void task_delete  ( );   
    void task_run     ( );
    void task_sleep   ( portTickType xTicksToDelay );
    
  protected:  

    unsigned long get_sys_tick();
      
    // function's   
    IxDriver( portCHAR * pcName );   
    
    virtual void CommandProcessor( TCommand &Command ) = 0;
    virtual void ThreadProcessor ( ) = 0;
    
    unsigned short DrvID;                                                       // this is CRC of drivers name
    unsigned short ConsumerID;                                                  // consumer ID which is connected currently to this driver.    
         
  private:  
    
    void registration();
    
    void DrvProcessor();
    
    void RUN(); 
    static void thRunnableFunction( void *pvParameters );
    
    portBASE_TYPE create_thread( );  
        
    portCHAR  pcDrvName[ configMAX_TASK_NAME_LEN ];    
    xTaskHandle taskdID;    
    
    IxDriver( const IxDriver & );
    IxDriver & operator=( const IxDriver & );    

    unsigned short initAttempt;    
                 
 }; typedef IxDriver *pIxDriver;

//------------------------------------------------------------------------------

#endif // _IX_DRIVER

