#include "utils.h"
#include "AT91SAM7S128.h"
#include "at91sam7s128\lib_AT91SAM7S.h"
#include "CxSysTimer.h"



//------------------------------------------------------------------------------
CxTSysBase &tSysBase= CxTSysBase::getInstance();
//------------------------------------------------------------------------------
TListOfVirtualTimers ListOfVirtualTimers;  
//------------------------------------------------------------------------------

CxSysVirtualTimer::CxSysVirtualTimer(void (*execution_func)(), unsigned long period, TMode mode, TState state):
   mCountConst  ( period )
  ,exec_func    ( execution_func ) 
  ,mWork        ( false )  
  ,mCycle       ( mode ) 
{
    AddToList();  
    
    if( state == CxSysVirtualTimer::on )
    {
      Start();
    }  
}

void CxSysVirtualTimer::Start()  
{
  mCount = mCountConst >> 2; 
  
  mWork = true;
} 
   
CxSysVirtualTimer::~CxSysVirtualTimer()       
{

}

//------------------------------------------------------------------------------

void  CxSysVirtualTimer::AddToList()
 {
   pCxSysVirtualTimer CurrentTmr;                                                        // pointer on next virtual timer 
   
   CurrentTmr = this;
   CurrentTmr->pNextTmr = NULL;
   if( ListOfVirtualTimers.CounterTimer == 0 )
   {
     ListOfVirtualTimers.FirstElem =  ListOfVirtualTimers.LastElem = CurrentTmr;
   }
   else
   {
     ListOfVirtualTimers.LastElem->pNextTmr = CurrentTmr;
     ListOfVirtualTimers.LastElem = CurrentTmr;
   }

   ListOfVirtualTimers.CounterTimer++;     
 }

//------------------------------------------------------------------------------

void  CxSysVirtualTimer::Dec()
 {
   if( mCount != 0)
   {
     if(--mCount == 0)
     {
       Over();
     }  
   }
 }
 
//------------------------------------------------------------------------------

void  CxSysVirtualTimer::Over()
 {
   mWork = false;
   if( NULL != exec_func )(*exec_func)();
   if( mCycle == CxSysVirtualTimer::cycle ) Start();   
 }
 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
 
CxTSysBase::CxTSysBase()
{

}

CxTSysBase &CxTSysBase::getInstance()
{
  static CxTSysBase theInstance;
  return theInstance;
}

void CxTSysBase::SysTic()
{
  unsigned char iteration=0;
  pCxSysVirtualTimer CurrentTmr;          // pointer on next virtual timer 

  if( ListOfVirtualTimers.CounterTimer != 0 )
  {
    CurrentTmr = ListOfVirtualTimers.FirstElem;    
    while( CurrentTmr->pNextTmr != NULL )
    {
      if( CurrentTmr->mWork )
      {
        CurrentTmr->Dec();
      }  
      CurrentTmr = CurrentTmr->pNextTmr;
      
      if( iteration > ListOfVirtualTimers.CounterTimer )
      {
        return;
      }
      iteration++;
     }
     
    CurrentTmr->Dec();
  }  
}

//------------------------------------------------------------------------------
// Hook functions

void vApplicationTickHook( void )
{
  tSysBase.SysTic(); 
}
