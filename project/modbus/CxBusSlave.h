#ifndef _CX_BUS_SLAVE
#define _CX_BUS_SLAVE

//------------------------------------------------------------------------------

#include "IxRunnable.h"
#include "CxBusHelper.h"
#include "..\framework\devctrl\CxLogDevice.h"
#include "..\framework\eventpool\IxEventConsumer.h"

//------------------------------------------------------------------------------
#define MY_ADDRESS     0x25
#define MB_TABEL_SIZE  100
#define IO_BUFFER_SIZE 101
//------------------------------------------------------------------------------

class CxBusSlave : public IxRunnable, public CxLogDevice, public IxEventConsumer
{
 public: 
   
    static CxBusSlave& getInstance( );    
    
    virtual void  Start();

 private:   

    CxBusSlave();
    ~CxBusSlave( ){}

    void MBusReadProcessor();
    void MBusWriteProcessor();
    
    // task processor
    virtual void TaskProcessor( );                                              // from IxRunnable
    
    virtual bool processEvent( pTEvent pEvent );                                // form IxEventConsumer
   
    char ioBuffer[IO_BUFFER_SIZE];
    bool newRequestWasResived;

    unsigned short MBTABLE[MB_TABEL_SIZE];
    
    CxBusHelper BusHelper;
}; 

typedef CxBusSlave *pCxBusSlave;

#endif /*_CX_BUS_SLAVE*/   
