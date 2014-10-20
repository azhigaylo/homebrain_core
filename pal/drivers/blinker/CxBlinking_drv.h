#ifndef _CX_BLINKING_DRV
#define _CX_BLINKING_DRV

//------------------------------------------------------------------------------
#include "IxDriver.h"
//------------------------------------------------------------------------------

class CxBlinking_drv : public IxDriver
{
  public:  

     // function's        
     static CxBlinking_drv& getInstance();
     
  protected:  
     
  private:     
    
     CxBlinking_drv();       
     ~CxBlinking_drv();      
    
     virtual void CommandProcessor( TCommand &Command );
     virtual void ThreadProcessor ( ){ }

 }; typedef CxBlinking_drv *pCxBlinking_drv;
 
//------------------------------------------------------------------------------

#endif // _CX_SYSTEM

