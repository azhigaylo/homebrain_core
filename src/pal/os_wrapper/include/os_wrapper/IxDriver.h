#ifndef _IX_DRIVER
#define _IX_DRIVER

//------------------------------------------------------------------------------
#include <pthread.h>
#include <time.h>
//------------------------------------------------------------------------------
#include "common/ptypes.h"
#include "common/utils.h"
#include "os_wrapper/CxQueue.h"
//------------------------------------------------------------------------------
#define configMAX_DRIVER_NAME_LEN 50
//------------------------------------------------------------------------------
// command type
#define identification_request  1
#define identification_response 2
#define request                 3
#define response                4

// common command ID
#define CM_DIReq                1
#define CM_DIRes                2
//------------------------------------------------------------------------------
struct TCommand
{
  uint16_t SenderID;
  uint16_t ConsumerID;
  uint16_t ComType;
  uint16_t ComID;
  void     *Container;
};
//------------------------------------------------------------------------------

class IxDriver
{
  public:

    virtual ~IxDriver();

    void driver_run();
    void driver_stop();

    // get time in s from thread start
    int64_t get_time();

  protected:

    virtual void CommandProcessor( uint16_t ComID, void *data ) = 0;
    virtual void ThreadProcessor ( );
    void sendMsg( uint16_t ComID, void *data );

    // we hide it because everybody should inherit it !
    IxDriver( const char *pcName );

    void task_delete( );
    void comm_task_delete( );

    uint16_t DrvID;                       // this is CRC of drivers name
    uint16_t ConsumerID;                  // consumer ID which is connected currently to this driver.

  private:

    int32_t create_thread( );
    int32_t create_comm_thread( );

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

    bool      interrupt_work;
    bool      interrupt_comm;

 }; typedef IxDriver *pIxDriver;

//------------------------------------------------------------------------------

#endif // _IX_DRIVER

