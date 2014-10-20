#ifndef _CX_ASSERTION
#define _CX_ASSERTION


//----------------------only wraper for FREERTOS xQueue ------------------------


#include "FreeRTOS.h"
#include "task.h"

//------------------------------------------------------------------------------
#define AssertionLength 30
#define AssertionStack  "[A] Stack failure"
#define AssertionTask   "[A] Task  failure"
#define AssertionMutex  "[A] Mutex failure"
#define AssertionQueue  "[A] Queue failure"
#define AssertionPool   "[A] Pool  failure"
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
   void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
   void vApplicationTaskCreateFailedHook(void);
   void vApplicationMutexCreateFailedHook(void);
   void vApplicationQueueCreateFailedHook(void); 
   void vApplicationStaticPoolFailedHook(void); 
#ifdef __cplusplus
}
#endif

class CxAssertion
{
 public:
  
   CxAssertion( );
   ~CxAssertion( );

   static CxAssertion & getInstance();
   
   void CallAssertion( const char* textOfAssertion ); 
   void CallAssertion( const char* textOfAssertion, const char* textAdditional ); 
   
 private:

   CxAssertion( const CxAssertion& rhs );
   CxAssertion& operator=( const CxAssertion& rhs );

   void ApplicationHalt();
   
   void LowUSARTinit();
   void PrintAssertionInUART();
   void write_char_USART0(unsigned char ch);
   void write_str_USART0(unsigned char* buff);  

   // data conteyner for assertion         
   char dataAssertion[AssertionLength];  
};

#endif

