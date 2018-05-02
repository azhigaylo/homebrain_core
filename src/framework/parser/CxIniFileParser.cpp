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
#include "parser/CxIniFileParser.h"

//------------------------------------------------------------------------------
//-----------------------CREATE INI FILE PARSER---------------------------------
//------------------------------------------------------------------------------

CxIniFileParser::CxIniFileParser():
   index(0)
  ,stateFSM(ST_LOOK_SECTION_START)
{

}

CxIniFileParser::~CxIniFileParser()
{

}

bool CxIniFileParser::ParseBuffer(const char *pBuff, int32_t dwLen)
{
   for(int32_t i = 0; i < dwLen; i++)
   {
     if( true == ProcessData(pBuff[i]) ) return true;
   }
   return false;
}

bool CxIniFileParser::ProcessData(uint8_t btData)
{
   switch(stateFSM)
   {
      // Search for start of section '['
      case ST_LOOK_SECTION_START :
                if(btData == '[')
                {
                   index = 0;                                               // reset index
                   stateFSM = ST_LOOK_SECTION_END;
                }
      break;

      // Search for end of section '[' and recieve section name
      case ST_LOOK_SECTION_END :
                if(btData != ']')
                {
                   section[index++] = btData;
                   // Check for command overflow
                   if(index >= SECLENGTH)
                   {
                      stateFSM = ST_LOOK_SECTION_START;
                   }
                }
                else
                {
                   section[index] = '\0';                                   // terminate command
                   if( false == isItEtalonSection() ) stateFSM = ST_LOOK_SECTION_START;
                   else {index = 0; stateFSM = ST_LOOK_KEY;}                // goto get data state
                }
      break;

      case ST_LOOK_KEY :
                if( btData != '=' )
                {
                   if( (btData != '\n') && (btData != '\r') )
                   {
                      if( btData == '[' )
                      {
                         index = 0;
                         stateFSM = ST_LOOK_SECTION_END;
                         break;
                      }
                      key[index++] = btData;
                      // Check for command overflow
                      if(index >= KEYLENGTH)
                      {
                         stateFSM = ST_LOOK_SECTION_START;
                      }
                   }
                }
                else
                {
                   key[index] = '\0';                                        // terminate command
                   index = 0;
                   if( false == isItEtalonKey() ) stateFSM = ST_LOOK_VALUE;
                   else {stateFSM = ST_LOOK_OUR_VALUE;}                      // goto get data state
                }
      break;

      case ST_LOOK_VALUE :
                if( (btData != '\n') && (btData != '\r') )
                {
                   value[index++] = btData;
                   // Check for command overflow
                   if(index >= VALUELENGTH)
                   {
                      stateFSM = ST_LOOK_SECTION_START;
                   }
                }
                else
                {
                   value[index] = '\0';                                      // terminate command
                   index = 0;
                   stateFSM = ST_LOOK_KEY;                                   // goto get data state
                }
      break;

      case ST_LOOK_OUR_VALUE :
                if( (btData != '\n') && (btData != '\r') )
                {
                   value[index++] = btData;
                   // Check for command overflow
                   if(index >= VALUELENGTH)
                   {
                      stateFSM = ST_LOOK_SECTION_START;
                   }
                }
                else
                {
                   value[index] = '\0';                                     // terminate command
                   stateFSM = ST_LOOK_SECTION_START;                        // goto get "[" state
                   return true;
                }
      break;

      default : stateFSM = ST_LOOK_SECTION_START;
   }
   return false;
}

void CxIniFileParser::SetEtalonSection( const char* sect )
{
  memset( etalonSection, 0, SECLENGTH );
  memcpy( etalonSection, sect, strnlen(sect, SECLENGTH) );
}

void CxIniFileParser::SetEtalonKey( const char* k )
{
  memset( etalonKey, 0, KEYLENGTH );
  memcpy( etalonKey, k, strnlen(k, KEYLENGTH) );
}

bool CxIniFileParser::isItEtalonSection()
{
  if( 0 == strcmp(section, etalonSection) ) return true;
  return false;
}

bool CxIniFileParser::isItEtalonKey()
{
  if( 0 == strcmp(key, etalonKey) ) return true;
  return false;
}

//------------------------------------------------------------------------------

bool CxIniFileParser::ReadBool( const char *IniFileName, const char*sect, const char*id, bool defoult )
{
   int32_t read_length = true;
   bool result = defoult;

   int ID = open( IniFileName, O_RDONLY );

   if( -1 != ID )
   {
      SetEtalonSection(sect);
      SetEtalonKey(id);

      while( 0 != read_length )
      {
         if ( (read_length = static_cast<uint32_t>(read( ID, tmp_ini_buf, INI_BUF_SIZE ))) == -1)
         {
            printError("CxIniFileParser/%s: read inifile error happened ", __FUNCTION__);
            break;
         }

         if( true == ParseBuffer(tmp_ini_buf, read_length) )
         {
            result = static_cast<bool>( atoi(GetValue()) );
            break;
         }
      }
      close( ID );
   }
   else
   {
      printError("CxIniFileParser/%s: open inifile=%s error!!! ", __FUNCTION__, IniFileName);
   }
   return result;
}

int32_t CxIniFileParser::ReadInt( const char *IniFileName, const char*sect, const char*id, int32_t defoult )
{
   int32_t read_length = true;
   int32_t  result = defoult;

   int ID = open( IniFileName, O_RDONLY );

   if( -1 != ID )
   {
      SetEtalonSection(sect);
      SetEtalonKey(id);

      while( 0 != read_length )
      {
         if ( (read_length = static_cast<uint32_t>(read( ID, tmp_ini_buf, INI_BUF_SIZE ))) == -1)
         {
            printError("CxIniFileParser/%s: read inifile error happened ", __FUNCTION__);
            break;
         }

         if( true == ParseBuffer(tmp_ini_buf, read_length) )
         {
            result = static_cast<int32_t>( atol(GetValue()) );
            break;
         }
      }
      close( ID );
   }
   else
   {
      printError("CxIniFileParser/%s: open inifile=%s error!!! ", __FUNCTION__, IniFileName);
   }

   return result;
}

float CxIniFileParser::ReadFloat( const char *IniFileName, const char*sect, const char*id, float defoult )
{
   int32_t read_length = true;
   float result = defoult;

   int ID = open( IniFileName, O_RDONLY );

   if( -1 != ID )
   {
      SetEtalonSection(sect);
      SetEtalonKey(id);

      while( 0 != read_length )
      {
         if ( (read_length = static_cast<uint32_t>(read( ID, tmp_ini_buf, INI_BUF_SIZE ))) == -1)
         {
            printError("CxIniFileParser/%s: read inifile error happened ", __FUNCTION__);
            break;
         }

         if( true == ParseBuffer(tmp_ini_buf, read_length) )
         {
            result = static_cast<float>( atof(GetValue()) );
            break;
         }
      }
      close( ID );
   }
   else
   {
      printError("CxIniFileParser/%s: open inifile=%s error!!! ", __FUNCTION__, IniFileName);
   }

   return result;
}

char* CxIniFileParser::ReadString( const char *IniFileName, const char*sect, const char*id )
{
   int32_t read_length = true;
   char* result = NULL;

   int ID = open( IniFileName, O_RDONLY );

   if( -1 != ID )
   {
      SetEtalonSection(sect);
      SetEtalonKey(id);

      while( 0 != read_length )
      {
         if ( (read_length = static_cast<uint32_t>(read( ID, tmp_ini_buf, INI_BUF_SIZE ))) == -1)
         {
            printError("CxIniFileParser/%s: read inifile error happened ", __FUNCTION__);
            break;
         }

         if( true == ParseBuffer(tmp_ini_buf, read_length) )
         {
            result = GetValue();
            break;
         }
      }
      close( ID );
   }
   else
   {
      printError("CxIniFileParser/%s: open inifile=%s error!!! ", __FUNCTION__, IniFileName);
   }

   return result;
}

