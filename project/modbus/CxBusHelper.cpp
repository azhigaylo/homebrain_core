#include "PSysI.h"
#include "CxBusHelper.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------

CxBusHelper::CxBusHelper( unsigned short* pMB ):
   dataProvider( CxDataProvider::getInstance() )
  ,DRouteList( d_point_total )   
  ,ARouteList( a_point_total )  
  ,TRouteList( t_point_total )  
  ,pMBTABLE( pMB )  
{    
   setNotification( CxEvent::EVENT_DP_NEW_VALUE );
   setNotification( CxEvent::EVENT_AP_NEW_VALUE );
   setNotification( CxEvent::EVENT_TP_NEW_VALUE );  
}

//------------------------------------------------------------------------------

bool CxBusHelper::processEvent( pTEvent pEvent )
{
  DBG_SCOPE( CxBusHelper, CxBusHelper )   
          
  // value event processing  
  if( pEvent->eventType == CxEvent::EVENT_DP_NEW_VALUE )
  {    
    pTDPOINT pDPOINT = reinterpret_cast<pTDPOINT>(pEvent->eventData); 
    DBG_MSG( ("[M] CxBusHelper: DP-%b = %w \n\r", pDPOINT->number, pDPOINT->value ) );
    return true;
  }
  
  if( pEvent->eventType == CxEvent::EVENT_AP_NEW_VALUE )
  {    
    pTAPOINT pAPOINT = reinterpret_cast<pTAPOINT>(pEvent->eventData);
    DBG_MSG( ("[M] CxBusHelper: AP-%b = %.2f \n\r", pAPOINT->number, pAPOINT->value ) );
    return true;
  }
  
  if( pEvent->eventType == CxEvent::EVENT_TP_NEW_VALUE )
  {    
    pTTEXTPOINT pTEXTPOINT = reinterpret_cast<pTTEXTPOINT>(pEvent->eventData); 
    DBG_MSG( ("[M] CxBusHelper: TP-%b =%s\n\r", pTEXTPOINT->number, pTEXTPOINT->text) );    
    return true;
  }  
  return false;
}

//------------------------------------------------------------------------------

bool CxBusHelper::CheckAndChangeInternalDataPoint( char regNumber )
{
  DBG_SCOPE( CxBusHelper, CxBusHelper )    
  // search in D routing table  
  for( short position = 0; position < DRouteList.count(); position++ )
  {
    TDRouteTableElement DRouteTableElement = DRouteList[position];
    if( regNumber == DRouteTableElement.rNumber )
    {      
      // set D internal point
      DBG_MSG( ("[M] CxBusHelper: pair DT-%w coincided \n\r", (unsigned short)position) );
      SetIntDescretPoint( regNumber, DRouteTableElement.dNumber );
      return true;
    }
  }   
  for( short position = 0; position < ARouteList.count(); position++ )
  {    
    TARouteTableElement ARouteTableElement = ARouteList[position];
    if( regNumber == ARouteTableElement.rNumber )
    {
      // set A internal point
      DBG_MSG( ("[M] CxBusHelper: pair AT-%w coincided \n\r", (unsigned short)position) );
      SetIntAnalogPoint( regNumber, ARouteTableElement.aNumber );
      return true;      
    }
  } 
  for( short position = 0; position < TRouteList.count(); position++ )
  {
    TTRouteTableElement TRouteTableElement = TRouteList[position];
    if( regNumber == TRouteTableElement.rNumber )
    {
      // set T internal point
      DBG_MSG( ("[M] CxBusHelper: pair TT-%w coincided \n\r", (unsigned short)position) );
      SetIntTextPoint( regNumber, TRouteTableElement.tNumber );
      return true;      
    }
  }   
  return false;
}

void CxBusHelper::setDRouteTableItem( char regNumber, char dpNumber )
{
  TDRouteTableElement DRouteTableElement = { regNumber, dpNumber };
  
  DRouteList.add( DRouteTableElement );
}

void CxBusHelper::setARouteTableItem( char regNumber, char apNumber )
{
  TARouteTableElement ARouteTableElement = { regNumber, apNumber };
  
  ARouteList.add( ARouteTableElement );
}

void CxBusHelper::setTRouteTableItem( char regNumber, char tpNumber )
{
  TTRouteTableElement TRouteTableElement = { regNumber, tpNumber };
  
  TRouteList.add( TRouteTableElement );
}
   
//------------------------------------------------------------------------------

bool CxBusHelper::SetIntDescretPoint( char regNumber, char dpNumber )
{
  // in the regNumber we have got discret point status in next register - value 
  dataProvider.setDPoint( dpNumber, getWordFromMbReg(pMBTABLE[regNumber + 1]) ); 
  dataProvider.setDStatus( dpNumber, getWordFromMbReg(pMBTABLE[regNumber]) );
  return true;
}

bool CxBusHelper:: SetIntAnalogPoint( char regNumber, char apNumber )
{
  dataProvider.setAPoint( apNumber, getFloatFromTwoMbReg(pMBTABLE[regNumber + 1], pMBTABLE[regNumber + 2]) ); 
  dataProvider.setDStatus( apNumber, getWordFromMbReg(pMBTABLE[regNumber]) );
  return true;
}

bool CxBusHelper:: SetIntTextPoint( char regNumber, char tpNumber )
{  
  dataProvider.putString( tpNumber, reinterpret_cast<char*>(&pMBTABLE[regNumber + 1]) );
  dataProvider.setTStatus( tpNumber, getWordFromMbReg(pMBTABLE[regNumber]) );
  return true;
}

//------------------------------------------------------------------------------
