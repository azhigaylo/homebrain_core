/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _CX_LOGDEV_EXT_MOD
#define _CX_LOGDEV_EXT_MOD

//------------------------------------------------------------------------------
#include <unistd.h>
#include <termios.h>

#include "common/ptypes.h"
#include "common/utils.h"
#include "devctrl/CxLogDevice.h"
#include "interfaces/CxModBusMaster.h"
#include "provider/CxDataProvider.h"
//------------------------------------------------------------------------------

#define SUPPORTED_REG_NUM 20

//------------------------------------------------------------------------------

#pragma pack(push, 1)
   // link record processor
   struct TLinkedReg
   {
     uint16_t strtReg;
     uint8_t  opType;
     uint16_t NPoint;
   };

   struct TContExtMod_USO
   {
     uint8_t  address;
     uint16_t usoPoint;
     uint16_t recNumb;
     TLinkedReg *pLinkedReg;  // array of the TLinkedReg
   };
#pragma pack(pop)

//------------------------------------------------------------------------------

class CxLogDev_ExtMod : public CxLogDevice, public CxSysTimer
{
   public:

      CxLogDev_ExtMod( const char *logDevName, const char *usedInterface, TContExtMod_USO modSettings );
      virtual ~CxLogDev_ExtMod();

      virtual bool Process();

   protected :

      virtual void sigHandler();

   private :

      TContExtMod_USO  dev_settings;                   // USO settings address and so on
      uint8_t          commError;                      // communication error with USO
      bool             recoveryFlag;                   // recovery output after start or after modbus error
      CxDataProvider   &dataProvider;                  // reference on the data provider
      CxModBusMaster   *pModBusMaster;                 // pointer to the interface
      uint16_t          firstRegRead;                  // min register number in registers list
      uint16_t          lastRegRead;                   // max register number in registers list
      uint16_t          mbResponce[SUPPORTED_REG_NUM]; // static buffer for modbus response

      bool ProcessAllRegisters();
      // process complex devices
      bool ReadWriteRegisters();
      // process simple devices
      bool ReadRegisters();
      bool WriteRegisters();
      // read and convert blok of parameters
      bool ReadRegisterBlock( uint16_t start_reg, uint16_t reg_count );

      // read and convert every parameters
      bool convertWordToApoint ( const TLinkedReg* pLinkedReg );
      bool convertLongToApoint ( const TLinkedReg* pLinkedReg );
      bool convertFloatToApoint( const TLinkedReg* pLinkedReg );
      bool convertHRegToDpoint ( const TLinkedReg* pLinkedReg );
      bool convertLRegToDpoint ( const TLinkedReg* pLinkedReg );

      bool convertApointToWord ( const TLinkedReg* pLinkedReg );
      bool convertApointToLong ( const TLinkedReg* pLinkedReg );
      bool convertApointToFloat( const TLinkedReg* pLinkedReg );
      bool convertDpointToReg  ( const TLinkedReg* pLinkedReg );

      void setExtModStatus( uint16_t status );

      CxLogDev_ExtMod( const CxLogDev_ExtMod & );
      CxLogDev_ExtMod & operator=( const CxLogDev_ExtMod & );
};

//------------------------------------------------------------------------------

#endif // _CX_LOGDEV_EXT_MOD

