#ifndef _CX_USOCFG_LOADER
#define _CX_USOCFG_LOADER

//------------------------------------------------------------------------------
#include "common/ptypes.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma pack(push, 1)

   struct TMODHEADER
   {
      uint8_t  Adress;
      uint8_t  PortN;
      uint16_t EMODpoint;
      uint8_t  RRecNumb;
      uint8_t  WRecNumb;
   };

   //--------------------------------------

   struct TPortMode
   {
     int8_t P1_MODE;
     int8_t P2_MODE;
     int8_t P3_MODE;
     int8_t P4_MODE;
   };

   struct TContAI_USO
   {
     uint8_t PortN;
     uint8_t Adress;
     uint8_t ChanN;
     uint16_t USOpoint;
   };// ttl  byte

   struct TContDIDO_USO
   {
     uint8_t PortN;
     uint8_t Adress;
     uint8_t ChanN;
     uint16_t USOpoint;
   };// ttl  byte


   struct TContMUK_USO
   {
     uint8_t PortN;
     uint8_t Adress;
     uint16_t USOpoint;
     uint16_t InfoBlkPoint;
     uint16_t ComBlkPoint;
   };// ttl  byte

   struct TContLO1111_USO
   {
     uint8_t PortN;
     uint8_t Adress;
     uint16_t USOpoint;
     uint16_t PowerPointNumb;
     uint16_t StealPointNumb;
   };// ttl  byte

   struct TConfigFileSP
   {
      TPortMode PortMode;
      int8_t nmb_AI_moduls;
      int8_t nmb_DIDO_moduls;
      int8_t nmb_tmp1_moduls;
      int8_t nmb_tmp2_moduls;
      int8_t nmb_MUK_moduls;
      int8_t nmb_LO1111_moduls;
      int8_t nmb_tmp[9];
      //----
      // ContAI_USO * nmb_AI_moduls
              //TAioChannel AioChannel * numb_ch from ContAI_USO;
      // ContDI16_USO * nmb_DI16_moduls
      // ContDI11_DO5_USO * nmb_DI11_DO5_moduls
      // ContDI16_DO4_USO * nmb_DI16_DO4_moduls
      // ContMUK_USO * nmb_MUK_moduls
   };

#pragma pack(pop)
//------------------------------------------------------------------------------

class CxUsoCfgLoader
{
   public:

      CxUsoCfgLoader();
      ~CxUsoCfgLoader();

      void Load( const char* cfg_path );

   private:

      void OpenExtModuleConfig( const char* cfg_path );
      void OpenAnalModuleConfig( const char* cfg_path );
      void OpenDioModuleConfig( const char* cfg_path );

      char* sUsoCgf_path;

}; typedef CxUsoCfgLoader *pTCxUsoCfgLoader;

#endif /*_CX_USOCFG_LOADER*/
