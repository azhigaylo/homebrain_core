#ifndef _CX_DRIVER_MANAGER
#define _CX_DRIVER_MANAGER

//------------------------------------------------------------------------------

#include "IxDriver.h"

//------------------------------------------------------------------------------

#define driverListSize 5

//------------------------------------------------------------------------------

struct TDrvRecord
{
   unsigned short DrvID;
   pIxDriver pDriver;
};
typedef TDrvRecord *pTDrvRecord;


//------------------------------------------------------------------------------


class CxDriverManager
{
 public: 

   static CxDriverManager & getInstance();
   
   bool driverRegistration( unsigned short DrvID, pIxDriver pDriver );   
   
   bool loadDriver( portCHAR * pcName );                                        // start on execution driver number = 0,1,... form DriverList
   
   void loadAllDrivers( );                                                      // start on execution all drivers from list DriverList
   
 private:
  
   CxDriverManager( ); 
   ~CxDriverManager(){}
   
   // loop timer 
   void StartTCO();                                                             // start TCO with 10uS tick interval
                                                                                // it's some time usefull in drivers
     
   TDrvRecord DriverList[driverListSize];

   char driverCounter;

   CxDriverManager( CxDriverManager const & other );
   CxDriverManager& operator=( CxDriverManager const & other );
   
}; typedef CxDriverManager * pCxDriverManager;


#endif /*_CX_DRIVER_MANAGER*/   
