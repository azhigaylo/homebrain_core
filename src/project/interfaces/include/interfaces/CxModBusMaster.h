/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_MBUS_MASTER
#define _CX_MBUS_MASTER

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "common/ptypes.h"
#include "common/utils.h"
#include "MBusDB.h"
#include "os_wrapper/CxThreadIO.h"
#include "interface/CxInterface.h"
#include "serial/CxSerialDriver.h"
//------------------------------------------------------------------------------

class CxModBusMaster : public CxThreadIO, public CxInterface
{
   public:

      CxModBusMaster( const char *interfaceName, const char *drvName );
      virtual ~CxModBusMaster();

      virtual int32_t open  ( );
      virtual int32_t close ( );

      uint16_t GetRegister( uint8_t address, uint16_t reg_start, uint16_t reg_count, uint16_t *pResponce );
      bool SetRegister( uint8_t address, uint16_t reg_numb, uint16_t reg_value );
      bool SetRegisterBlock( uint8_t address, uint16_t reg_start, uint16_t reg_count, const uint16_t *pOutput );

   protected :

      virtual void CommandProcessor( uint16_t ComID, void *data );

   private :

      uint16_t          counter_item;
      uint16_t          sizeResponce;
      uint8_t           last_command;
      uint8_t           last_address;

      TSerialBlock      commbuf;
      TMREQ             mbReadRequest;
      TMWRREG           mbWriteRequest;
      TMBWrBlkReg       mbWrBlkReg;
      TMRESP            mbResponce;

      // synchronization block
      pthread_barrier_t our_barrier;

      pthread_mutex_t cond_mutex;
      pthread_cond_t  cond_var;
      uint16_t        cond_var_flag; // if flag > 0 - we can go further

      void sleep_till_resp();
      void waikeup_by_serial();

      CxModBusMaster( const CxModBusMaster & );
      CxModBusMaster & operator=( const CxModBusMaster & );
};

//------------------------------------------------------------------------------

#endif // _CX_MBUS_MASTER

