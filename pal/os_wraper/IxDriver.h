#ifndef _IX_DRIVER
#define _IX_DRIVER

//------------------------------------------------------------------------------
#include <pthread.h>
#include <time.h>
//------------------------------------------------------------------------------
#include "ptypes.h"
#include "utils.h"
#include "CxQueue.h"
//------------------------------------------------------------------------------
#define configMAX_DRIVER_NAME_LEN 50
//------------------------------------------------------------------------------
// command type
#define identification_request  1
#define identification_response 2
#define request                 3
#define response                4

// common command ID
#define DIReq                   1
#define DIRes                   2
//------------------------------------------------------------------------------
struct TCommand
{
  uint16_t SenderID;  
  uint16_t ConsumerID;  
  uint16_t  ComType;
  uint16_t  ComID;
  void *Container;
};
//------------------------------------------------------------------------------

// driver idettification request format

class IxDriver
{ 
  public:

    ~IxDriver();
    // we hide it because everybody should inherit it !  
    IxDriver( const char *pcName );	

    void task_run();

    // get time in s from thread start
    uint64_t get_time();

  protected:  

    virtual void CommandProcessor( TCommand &Command ){} // = 0;
    virtual void ThreadProcessor ( );

    // we hide it because everybody should inherit it !  
    //IxDriver( const char *pcName );

    uint16_t DrvID;                       // this is CRC of drivers name
    uint16_t ConsumerID;                  // consumer ID which is connected currently to this driver.

  private:

    // we hide it because everybody should inherit it !  
    //IxDriver( const char *pcName );

    int32_t create_thread( );
    int32_t create_comm_thread( );

    void task_delete( );
    void comm_task_delete( );

    void run( );
    void run_comm( );
    static void * thRunnableFunction_IxDriver( void *args );
    static void * thRunnableCommFunction_IxDriver( void *args );

    void DrvProcessor( );
    void CommProcessor( );

    pthread_t workThreadID;
    pthread_t commThreadID;

    time_t    startTime;
    CxQueue   inQueue;
    CxQueue   outQueue;

    IxDriver( const IxDriver & );
    IxDriver & operator=( const IxDriver & );

    uint16_t  initAttempt;
    char      pcDrvName[configMAX_DRIVER_NAME_LEN];
    char      pcCommThreadName[configMAX_DRIVER_NAME_LEN];

 }; typedef IxDriver *pIxDriver;

//------------------------------------------------------------------------------

#endif // _IX_DRIVER

