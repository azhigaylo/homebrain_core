#ifndef _CX_LOG_DEV_MANAGER
#define _CX_LOG_DEV_MANAGER

//------------------------------------------------------------------------------

#include "CxLogDevice.h"
#include "..\staticpool\CxVector.h"

//------------------------------------------------------------------------------

#define logDevListSize 10

//------------------------------------------------------------------------------
#pragma pack ( 1 )
struct TLogDevListItem
{
  char number;
  pCxLogDevice pLogDevice;
};
#pragma pack ( )
//------------------------------------------------------------------------------

class CxLogDeviceManager
{
 public: 

   static CxLogDeviceManager & getInstance();
   
   bool logDevRegistration( pCxLogDevice pLogDevice );
   
   pCxLogDevice getLogDev( char number );                                       // get by number in vector - number = 0,1,...
   pCxLogDevice getLogDev( const char *deviceName );                            // get by name
   
   
 private:
  
   CxLogDeviceManager( ); 
   ~CxLogDeviceManager(){}
       
   CxVector<TLogDevListItem> LOGDEV_LIST;
   
   TLogDevListItem logDevListItemTmp;
     
   char logDevCounter;

   CxLogDeviceManager( CxLogDeviceManager const & other );
   CxLogDeviceManager& operator=( CxLogDeviceManager const & other );
   
}; typedef CxLogDeviceManager * pCxLogDeviceManager;


#endif /*_CX_LOG_DEV_MANAGER*/   
