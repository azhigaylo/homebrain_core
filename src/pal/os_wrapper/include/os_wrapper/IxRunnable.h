#ifndef _IX_RUNENABLED
#define _IX_RUNENABLED

//------------------------------------------------------------------------------
#include <pthread.h>
#include <time.h>
//------------------------------------------------------------------------------
#include "common/ptypes.h"
//------------------------------------------------------------------------------
#define configMAX_TASK_NAME_LEN 50
//------------------------------------------------------------------------------

class IxRunnable
{
  public:

    virtual ~IxRunnable();

    void task_run();
    void task_join();
    void task_stop();

  protected:

    // get time in s from thread start
    int64_t get_time();

    char pcTaskName[configMAX_TASK_NAME_LEN];

    pthread_t thread;
    time_t    start_time;
    bool      interrupt;

    // we hide it because everybody should inherit it !
    IxRunnable( const char *pcName );

    virtual void TaskProcessor();

  private:

    void run();

    static void * thRunnableFunction( void *args );

    int32_t create_thread( );

    IxRunnable( const IxRunnable & );
    IxRunnable & operator=( const IxRunnable & );

}; typedef IxRunnable *pIxRunnable;

//------------------------------------------------------------------------------

#endif // _IX_RUNENABLED

