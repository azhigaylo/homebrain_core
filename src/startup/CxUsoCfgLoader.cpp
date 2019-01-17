/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//------------------------------------------------------------------------------
#include "common/slog.h"
#include "common/utils.h"
#include "uso/USODefinition.h"
#include "uso/CxLogDev_MA16.h"
#include "uso/CxLogDev_DIO_OVEN.h"
#include "uso/CxLogDev_ExtMod.h"
#include "startup/CxUsoCfgLoader.h"

//------------------------------------------------------------------------------

CxUsoCfgLoader::CxUsoCfgLoader( ):
    sUsoCgf_path ( 0 )
{

}

CxUsoCfgLoader::~CxUsoCfgLoader()
{
    printDebug("CxUsoCfgLoader::%s: deleted...", __FUNCTION__);
}

void CxUsoCfgLoader::Load( const char* cfg_path )
{
   sUsoCgf_path = strdup(cfg_path);

   printDebug("CxUsoCfgLoader/%s: look for log dev in %s", __FUNCTION__, sUsoCgf_path);

   if (0 != sUsoCgf_path)
   {
      OpenExtModuleConfig( sUsoCgf_path );

      OpenAnalModuleConfig( sUsoCgf_path );

      OpenDioModuleConfig( sUsoCgf_path );

      free (sUsoCgf_path);
   }
}

void CxUsoCfgLoader::OpenExtModuleConfig( const char* cfg_path )
{
   const char *sExtModName = "ExtMod.bin";

   // allocate memory for name str
   char *sExtModConfig = (char*)malloc(strnlen(cfg_path,200) + strnlen(sExtModName,50));
   memset(sExtModConfig, 0, strnlen(cfg_path,200) + strnlen(sExtModName,50));
   // copy path
   strncpy( sExtModConfig, cfg_path, strnlen(cfg_path,200) );
   // copy name
   strcat(sExtModConfig, sExtModName);

   if (0 != sExtModConfig)
   {
      int ID = open( sExtModConfig, O_RDONLY );

      if( -1 != ID )
      {
         printDebug("CxUsoCfgLoader/%s: load %s", __FUNCTION__, sExtModConfig);

         //read link-file header
         uint8_t totalMod = 0;
         read( ID, &totalMod, sizeof(totalMod) );

         if (totalMod > 0)
         {
            // we found totalMod in the configuration file
            TMODHEADER CMODHEADER;
            int64_t offset = sizeof(totalMod)+ totalMod*sizeof(TMODHEADER);

            for(uint8_t modNum=0; modNum < totalMod; modNum++)
            {
               if (-1 != read( ID,(char*)&CMODHEADER, sizeof(TMODHEADER)))
               {
                  printDebug("CxUsoCfgLoader/%s: find module modNum=%d: %d %d %d %d %d", __FUNCTION__,
                                                                                         modNum,
                                                                                         CMODHEADER.Adress,
                                                                                         CMODHEADER.PortN,
                                                                                         CMODHEADER.EMODpoint,
                                                                                         CMODHEADER.RRecNumb,
                                                                                         CMODHEADER.WRecNumb );
                  // read linked data
                  uint32_t linkedSize = static_cast<uint32_t>((CMODHEADER.RRecNumb*sizeof(TLinkedReg) + CMODHEADER.WRecNumb*sizeof(TLinkedReg)));
                  TLinkedReg *pLinkedReg = new TLinkedReg[CMODHEADER.RRecNumb + CMODHEADER.WRecNumb];

                  if (-1 == lseek(ID,  offset, SEEK_SET))
                  {
                     printError("CxUsoCfgLoader/%s: lseek error=%s", __FUNCTION__, strerror(errno));
                     break;
                  }

                  if (-1 != read( ID, (char*)pLinkedReg, linkedSize))
                  {
                     // make a name
                     char *sCfgName = (char*)malloc(100);
                     sprintf( sCfgName, "LogDev_EXTM_%d", modNum);

                     // it's a bad hak, but i don't know how fix it
                     char *sInterfaceName = (char*)malloc(100);
                     sprintf( sInterfaceName, "mb_master_%d", CMODHEADER.PortN);

                     // create logical device
                     TContExtMod_USO contExtMod_USO = { CMODHEADER.Adress,
                                                        CMODHEADER.EMODpoint,
                                                        static_cast<uint16_t>(CMODHEADER.RRecNumb + CMODHEADER.WRecNumb),
                                                        pLinkedReg };

                     new CxLogDev_ExtMod( sCfgName, sInterfaceName, contExtMod_USO);             // this item will be deleted in CxLogDeviceManager::delInstance()

                     free(sCfgName);
                     free(sInterfaceName);
                  }
                  else
                  {
                     printDebug("CxUsoCfgLoader/%s: read LinkedReg, error=%s", __FUNCTION__, strerror(errno));
                  }

                  if (-1 == lseek(ID,  sizeof(totalMod)+(modNum+1)*sizeof(TMODHEADER), SEEK_SET))
                  {
                     printError("CxUsoCfgLoader/%s: lseek error=%s", __FUNCTION__, strerror(errno));
                     break;
                  }

                  offset += linkedSize;
               }
               else
               {
                  printError("CxUsoCfgLoader/%s: read %s, error=%s", __FUNCTION__, sExtModConfig, strerror(errno));
               }
            }
         }

         close( ID );
      }
      else
      {
         printDebug("CxUsoCfgLoader/%s: open %s, error=%s", __FUNCTION__, sExtModConfig, strerror(errno));
      }

      free(sExtModConfig);
   }
}

void CxUsoCfgLoader::OpenAnalModuleConfig( const char* cfg_path )
{
   const char *sDevMaName = "uso.bin";
   const char *sDummyName  = "LogDev_MA_";

   // allocate memory for name str
   char *sDevMaConfig = (char*)malloc(strnlen(cfg_path,200) + strnlen(sDevMaName,50));
   memset(sDevMaConfig, 0, strnlen(cfg_path,200) + strnlen(sDevMaName,50));
   // copy path
   strncpy( sDevMaConfig, cfg_path, strnlen(cfg_path,200) );
   // copy name
   strcat(sDevMaConfig, sDevMaName);

   if (0 != sDevMaConfig)
   {
      int ID = open( sDevMaConfig, O_RDONLY );

      if( -1 != ID )
      {
         printDebug("CxUsoCfgLoader/%s: load %s", __FUNCTION__, sDevMaConfig);

         union
         {
            TConfigFileSP ConfigFileSP; TAioChannel AioChannel; TDioChannel DioChannel; TContAI_USO ContAI_USO; TContDIDO_USO ContDIDO_USO; TContMUK_USO ContMUK_USO; TContLO1111_USO ContLO1111_USO;
         }CommonC;

         if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ConfigFileSP)) )
         {
           int8_t nmb_AI_moduls   = CommonC.ConfigFileSP.nmb_AI_moduls;

           // create AI module
           for (uint8_t modNum=0; modNum<nmb_AI_moduls; modNum++)
           {
               // read module description
               if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ContAI_USO)))
               {
                  printDebug("CxUsoCfgLoader/%s: find module modNum=%d: %d %d %d %d", __FUNCTION__,
                                                                                      modNum,
                                                                                      CommonC.ContAI_USO.Adress,
                                                                                      CommonC.ContAI_USO.PortN,
                                                                                      CommonC.ContAI_USO.USOpoint,
                                                                                      CommonC.ContAI_USO.ChanN );
                  // read channel description
                  uint32_t linkedSize = static_cast<uint32_t>(CommonC.ContAI_USO.ChanN * sizeof(CommonC.AioChannel));
                  TAioChannel *pAioChannel = new TAioChannel[CommonC.ContAI_USO.ChanN];

                  if (-1 != read( ID, (char*)pAioChannel, linkedSize))
                  {
                     // make a name
                     char *sCfgName = (char*)malloc(100);
                     sprintf( sCfgName, "%s%d", sDummyName, modNum);

                     char *sInterfaceName = (char*)malloc(100);
                     sprintf( sInterfaceName, "mb_master_%d", CommonC.ContAI_USO.PortN);

                     // create logical device
                     TAI_USO contAI_USO = { CommonC.ContAI_USO.Adress, CommonC.ContAI_USO.USOpoint, CommonC.ContAI_USO.ChanN, pAioChannel };

                     new CxLogDev_MA( sCfgName, sInterfaceName, contAI_USO);   // this item will be deleted in CxLogDeviceManager::delInstance()

                     free(sCfgName);
                     free(sInterfaceName);
                  }
                  else
                  {
                     printDebug("CxUsoCfgLoader/%s: read LinkedReg, error=%s", __FUNCTION__, strerror(errno));
                  }
               }
               else
               {
                  printError("CxUsoCfgLoader/%s: read %s, error=%s", __FUNCTION__, sDevMaConfig, strerror(errno));
               }
            }
         }
         close( ID );
      }
      else
      {
         printDebug("CxUsoCfgLoader/%s: open %s, error=%s", __FUNCTION__, sDevMaConfig, strerror(errno));
      }

      free(sDevMaConfig);
   }
}


void CxUsoCfgLoader::OpenDioModuleConfig( const char* cfg_path )
{
   const char *sDevMaName = "uso.bin";
   const char *sDummyName  = "LogDev_DIO_OVEN_";

   // allocate memory for name str
   char *sDevMaConfig = (char*)malloc(strnlen(cfg_path,200) + strnlen(sDevMaName,50));
   memset(sDevMaConfig, 0, strnlen(cfg_path,200) + strnlen(sDevMaName,50));
   // copy path
   strncpy( sDevMaConfig, cfg_path, strnlen(cfg_path,200) );
   // copy name
   strcat(sDevMaConfig, sDevMaName);

   if (0 != sDevMaConfig)
   {
      int ID = open( sDevMaConfig, O_RDONLY );

      if( -1 != ID )
      {
         printDebug("CxUsoCfgLoader/%s: load %s", __FUNCTION__, sDevMaConfig);

         union
         {
            TConfigFileSP ConfigFileSP; TAioChannel AioChannel; TDioChannel DioChannel; TContAI_USO ContAI_USO; TContDIDO_USO ContDIDO_USO; TContMUK_USO ContMUK_USO; TContLO1111_USO ContLO1111_USO;
         }CommonC;

         if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ConfigFileSP)) )
         {
            int8_t nmb_DIDO_moduls = CommonC.ConfigFileSP.nmb_DIDO_moduls;

           // create AI module
           for (uint8_t modNum=0; modNum<nmb_DIDO_moduls; modNum++)
           {
               // read module description
               if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ContDIDO_USO)))
               {
                  printDebug("CxUsoCfgLoader/%s: find module modNum=%d: %d %d %d %d", __FUNCTION__,
                                                                                      modNum,
                                                                                      CommonC.ContDIDO_USO.Adress,
                                                                                      CommonC.ContDIDO_USO.PortN,
                                                                                      CommonC.ContDIDO_USO.USOpoint,
                                                                                      CommonC.ContDIDO_USO.ChanN );
                  // read channel description
                  uint32_t linkedSize = static_cast<uint32_t>(CommonC.ContDIDO_USO.ChanN * sizeof(CommonC.DioChannel));
                  TDioChannel *pDioChannel = new TDioChannel[CommonC.ContDIDO_USO.ChanN];

                  if (-1 != read( ID, (char*)pDioChannel, linkedSize))
                  {
                     // make a name
                     char *sCfgName = (char*)malloc(100);
                     sprintf( sCfgName, "%s%d", sDummyName, modNum);

                     char *sInterfaceName = (char*)malloc(100);
                     sprintf( sInterfaceName, "mb_master_%d", CommonC.ContDIDO_USO.PortN);

                     // create logical device
                     TDIO_USO contDIO_USO = { CommonC.ContDIDO_USO.Adress, CommonC.ContDIDO_USO.USOpoint, CommonC.ContDIDO_USO.ChanN, pDioChannel };

                     new CxLogDev_DIO_OVEN( sCfgName, sInterfaceName, contDIO_USO);   // this item will be deleted in CxLogDeviceManager::delInstance()

                     free(sCfgName);
                     free(sInterfaceName);
                  }
                  else
                  {
                     printDebug("CxUsoCfgLoader/%s: read LinkedReg, error=%s", __FUNCTION__, strerror(errno));
                  }
               }
               else
               {
                  printError("CxUsoCfgLoader/%s: read %s, error=%s", __FUNCTION__, sDevMaConfig, strerror(errno));
               }
            }
         }

         close( ID );
      }
      else
      {
         printDebug("CxUsoCfgLoader/%s: open %s, error=%s", __FUNCTION__, sDevMaConfig, strerror(errno));
      }

      free(sDevMaConfig);
   }
}
