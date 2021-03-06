/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_LOG_DEV_MANAGER
#define _CX_LOG_DEV_MANAGER

//------------------------------------------------------------------------------
#include <vector>

#include "common/slog.h"
#include "common/utils.h"
#include "os_wrapper/CxMutex.h"
#include "IxLogDevice.h"
//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TLogDevListItem
{
  uint16_t number;
  IxLogDevice *pLogDevice;
};
#pragma pack ( )
//------------------------------------------------------------------------------

class CxLogDeviceManager
{
 public:

   static CxLogDeviceManager * getInstance();
   static void delInstance();

   bool set_logdev( IxLogDevice * pLogDev );
   IxLogDevice *get_logdev( const char *name );
   IxLogDevice *get_logdev_by_number( uint16_t numb );

   void process_all( );

 private:

   CxLogDeviceManager( );
   ~CxLogDeviceManager(){}

   void clr_logdev_list();

   std::vector<TLogDevListItem> LOGDEV_LIST;
   TLogDevListItem logDevListItemTmp;

   static CxMutex singlDeviceLock;
   static CxLogDeviceManager* theInstance;

   CxLogDeviceManager( CxLogDeviceManager const & other );
   CxLogDeviceManager& operator=( CxLogDeviceManager const & other );

}; typedef CxLogDeviceManager * pCxLogDeviceManager;


#endif /*_CX_LOG_DEV_MANAGER*/
