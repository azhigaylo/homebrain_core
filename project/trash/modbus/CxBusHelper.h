#ifndef _CX_BUS_HELPER
#define _CX_BUS_HELPER

//------------------------------------------------------------------------------

#include "..\staticpool\CxVector.h"
#include "..\framework\provider\CxDataProvider.h"
#include "..\framework\eventpool\IxEventConsumer.h"

//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TDRouteTableElement
{
  char rNumber;
  char dNumber;
}; typedef TDRouteTableElement *pTDRouteTableElement; 

struct TARouteTableElement
{
  char rNumber;
  char aNumber;
}; typedef TARouteTableElement *pTARouteTableElement;

struct TTRouteTableElement
{    
  char rNumber;
  char tNumber;
}; typedef TTRouteTableElement *pTTRouteTableElement; 
#pragma pack ( )

//------------------------------------------------------------------------------

class CxBusHelper : public IxEventConsumer
{ 
 public: 
   
   CxBusHelper( unsigned short* pMB );
   ~CxBusHelper( ){}   
   
   bool CheckAndChangeInternalDataPoint( char regNumber );
   
   void setDRouteTableItem( char regNumber, char dpNumber );
   void setARouteTableItem( char regNumber, char apNumber );
   void setTRouteTableItem( char regNumber, char tpNumber );

 private:   
            
    CxDataProvider & dataProvider;
    
    virtual bool processEvent( pTEvent pEvent );                                // form IxEventConsumer
    
    bool SetIntDescretPoint( char regNumber, char dpNumber );      
    bool SetIntAnalogPoint( char regNumber, char apNumber );
    bool SetIntTextPoint( char regNumber, char tpNumber );  
            
    CxVector<TDRouteTableElement> DRouteList;
    CxVector<TARouteTableElement> ARouteList;
    CxVector<TTRouteTableElement> TRouteList;
    
    unsigned short* pMBTABLE;
}; 

typedef CxBusHelper *pCxBusHelper;

#endif /*_CX_BUS_HELPER*/   
