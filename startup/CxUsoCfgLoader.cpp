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
#include "slog.h"
#include "utils.h"
#include "USODefinition.h"
#include "CxLogDev_MA16.h"
#include "CxLogDev_ExtMod.h"
#include "CxUsoCfgLoader.h"

//------------------------------------------------------------------------------

CxUsoCfgLoader::CxUsoCfgLoader( ):
    sUsoCgf_path ( 0 )
{

}

CxUsoCfgLoader::~CxUsoCfgLoader()
{

}

void CxUsoCfgLoader::Load( const char* cfg_path )
{
   sUsoCgf_path = strdup(cfg_path);

   printDebug("CxUsoCfgLoader/%s: look for log dev in %s", __FUNCTION__, sUsoCgf_path);

   if (0 != sUsoCgf_path)
   {
      OpenExtModuleConfig( sUsoCgf_path );

      OpenAnalModuleConfig( sUsoCgf_path );

      free (sUsoCgf_path);
   }
}

void CxUsoCfgLoader::OpenExtModuleConfig( const char* cfg_path )
{
   const char *sExtModName = "/ExtMod.lnk";

   // allocate memory for name str
   char *sExtModConfig = (char*)malloc(strlen_m(const_cast<char*>(cfg_path),200) + strlen_m(const_cast<char*>(sExtModName),50));
   // copy path 
   strncpy_m( sExtModConfig, const_cast<char*>(cfg_path), strlen_m(const_cast<char*>(cfg_path),200) );
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
            uint64_t offset = sizeof(totalMod)+ totalMod*sizeof(TMODHEADER);

            for(uint8_t modNum=0; modNum < totalMod; modNum++)
            {
               //
               if (-1 != read( ID,(char*)&CMODHEADER, sizeof(TMODHEADER)))
               {
                  printDebug("CxUsoCfgLoader/%s: find module modNum=%d: %d %d %d %d %d", __FUNCTION__, modNum, CMODHEADER.Adress, CMODHEADER.PortN, CMODHEADER.EMODpoint, CMODHEADER.RRecNumb, CMODHEADER.WRecNumb );
                  // read linked data
                  uint32_t linkedSize = (CMODHEADER.RRecNumb*sizeof(TLinkedReg) + CMODHEADER.WRecNumb*sizeof(TLinkedReg));
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
                     
                     char *sInterfaceName = (char*)malloc(100);
                     sprintf( sInterfaceName, "mbus_master_%d", CMODHEADER.PortN);                     

                     // create logical device
                     TContExtMod_USO contExtMod_USO = { CMODHEADER.Adress, CMODHEADER.EMODpoint, CMODHEADER.RRecNumb + CMODHEADER.WRecNumb, pLinkedReg };

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
   const char *sDevMaName = "/uso.cnf";
   const char *sDummyName  = "LogDev_MA_";

   // allocate memory for name str
   char *sDevMaConfig = (char*)malloc(strlen_m(const_cast<char*>(cfg_path),200) + strlen_m(const_cast<char*>(sDevMaName),50));
   // copy path
   strncpy_m( sDevMaConfig, const_cast<char*>(cfg_path), strlen_m(const_cast<char*>(cfg_path),200) );
   // copy name
   strcat(sDevMaConfig, sDevMaName);

   if (0 != sDevMaConfig)
   {
      int ID = open( sDevMaConfig, O_RDONLY );

      if( -1 != ID )
      {
         printDebug("CxUsoCfgLoader/%s: load %s", __FUNCTION__, sDevMaConfig);

         int8_t nmb_AI_moduls = 0, nmb_DIDO_moduls = 0, nmb_MUK_moduls = 0, nmb_LO1111_moduls = 0;

         union
         {
            TConfigFileSP ConfigFileSP; TAioChannel AioChannel; TDioChannel DioChannel; TContAI_USO ContAI_USO; TContDIDO_USO ContDIDO_USO; TContMUK_USO ContMUK_USO; TContLO1111_USO ContLO1111_USO;
         }CommonC;
         
         if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ConfigFileSP)) )
         {
            nmb_AI_moduls   = CommonC.ConfigFileSP.nmb_AI_moduls;
            nmb_DIDO_moduls = CommonC.ConfigFileSP.nmb_DIDO_moduls;
            nmb_MUK_moduls  = CommonC.ConfigFileSP.nmb_MUK_moduls;
            nmb_LO1111_moduls = CommonC.ConfigFileSP.nmb_LO1111_moduls;      

           // create AI module 
           for (uint8_t modNum=0; modNum<nmb_AI_moduls; modNum++)
           {
               // read module description
               if (-1 != read( ID,(char*)&CommonC, sizeof(CommonC.ContAI_USO)))
               {
                  printDebug("CxUsoCfgLoader/%s: find module modNum=%d: %d %d %d", __FUNCTION__, modNum, CommonC.ContAI_USO.Adress, CommonC.ContAI_USO.USOpoint, CommonC.ContAI_USO.ChanN );

                  // read channel description   
                  uint32_t linkedSize = CommonC.ContAI_USO.ChanN * sizeof(CommonC.AioChannel);
                  TAioChannel *pAioChannel = new TAioChannel[CommonC.ContAI_USO.ChanN];

                  if (-1 != read( ID, (char*)pAioChannel, linkedSize))
                  {
                     // make a name
                     char *sCfgName = (char*)malloc(100);
                     sprintf( sCfgName, "%s%d", sDummyName, modNum);

                     char *sInterfaceName = (char*)malloc(100);
                     sprintf( sInterfaceName, "mbus_master_%d", CommonC.ContAI_USO.PortN);                     

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
           // another module 
           // ...
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
