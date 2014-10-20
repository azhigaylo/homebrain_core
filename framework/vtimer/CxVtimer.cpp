//------------------------------------------------------------------------------
//---------------------class for virtual timer's--------------------------------
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "CxVtimer.h"
#include "..\debug\DebugMacros.h"

TListTimer ListTimer;  
CxTBase &tBase= CxTBase::getInstance();
//------------------------------------------------------------------------------

CxVTimer::CxVTimer(unsigned long period, TMode mode, TState state):
   mCountConst  ( period )
  ,exec_func    ( NULL   ) 
  ,mWork        ( false  )  
  ,mCycle       ( mode   )
  ,pTimerEventConsumer ( NULL )    
{    
  AddToList();  
    
  if( state==CxVTimer::on )
  {
    Start();      
  }  
  
  timerID = rand();
}

CxVTimer::CxVTimer(void (*execution_func)(), unsigned long period, TMode mode, TState state):
   mCountConst  ( period )
  ,exec_func    ( execution_func ) 
  ,mWork        ( false )  
  ,mCycle       ( mode ) 
  ,pTimerEventConsumer ( NULL )  
{
    AddToList();  
    
    if( state==CxVTimer::on )
    {
      Start();
    }  
    
    timerID = rand();    
}

CxVTimer::CxVTimer( pIxTimerEventConsumer consumer, unsigned long period, TMode mode, TState state ):
   mCountConst  ( period )
  ,exec_func    ( NULL   ) 
  ,mWork        ( false  )  
  ,mCycle       ( mode   )    
  ,pTimerEventConsumer ( consumer )    
{
    AddToList();  
    
    if( state==CxVTimer::on )
    {
      Start();
    }  
    
    timerID = rand();    
}

void CxVTimer::Start()  
{
  mCount = (mCountConst >> 2) + tBase.get_sys_tick(); 
  
  mWork = true;
} 
   
CxVTimer::~CxVTimer()       
{

}

//------------------------------------------------------------------------------

void  CxVTimer::AddToList()
 {
   pCxVTimer CurrentTmr;                                                        // pointer on next virtual timer 
   
   CurrentTmr = this;
   CurrentTmr->NextTmr = NULL;
   if( ListTimer.CounterTimer == 0 )
   {
     ListTimer.FirstElem =  ListTimer.LastElem = CurrentTmr;
   }
   else
   {
     ListTimer.LastElem->NextTmr = CurrentTmr;
     ListTimer.LastElem = CurrentTmr;
   }

   ListTimer.CounterTimer++;     
 }

//------------------------------------------------------------------------------
 
unsigned short CxVTimer::GetTimerID()
{
  return timerID;
}

//------------------------------------------------------------------------------

void CxVTimer::TimerCheck()
{
   if( mWork == true )
   {
     if( tBase.get_sys_tick() > mCount ) Over();
   }
}

//------------------------------------------------------------------------------

void  CxVTimer::Over()
 {
   mWork = false;
   if( NULL != exec_func )(*exec_func)();
   if( NULL != pTimerEventConsumer )pTimerEventConsumer->TimerEventProcessor( timerID );
   if( mCycle == CxVTimer::cycle ) Start();   
 }
 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CxTBase::CxTBase():
   IxRunnable ( "VT_TASK" ) 
{

}

CxTBase &CxTBase::getInstance()
{
  static CxTBase theInstance;
  return theInstance;
}

void CxTBase::SysTic()
{
  unsigned char iteration=0;
  pCxVTimer CurrentTmr;          // pointer on next virtual timer 

  if( ListTimer.CounterTimer != 0 )
  {
    CurrentTmr = ListTimer.FirstElem;    
    while( CurrentTmr->NextTmr != NULL )
    {
      if( CurrentTmr->mWork )
      {
        CurrentTmr->TimerCheck();
      }  
      CurrentTmr = CurrentTmr->NextTmr;
      
      if( iteration > ListTimer.CounterTimer )
      {
        return;
      }
      iteration++;
     }
     
    CurrentTmr->TimerCheck();
  }    
}

void CxTBase::TaskProcessor()
{
  SysTic();  
}
