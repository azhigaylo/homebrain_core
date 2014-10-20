#ifndef _CX_TWI_TEST
#define _CX_TWI_TEST

//------------------------------------------------------------------------------

#include "IxRunnable.h"
#include "pio_it.h"

//------------------------------------------------------------------------------

class CxTwiTest : public IxRunnable
{
  public:
    
    static CxTwiTest& getInstance();    

    virtual void TaskProcessor();

  protected:
    
    CxTwiTest();
    ~CxTwiTest(){}
    
    Pin pinDataA2;
    Pin pinDataA5;

}; typedef CxTwiTest *TCxTwiTest;

#endif /*_CX_TWI_TEST*/
