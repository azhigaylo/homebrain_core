#include "CxIniFileParser.h"
#include <stdlib.h>
#include "Utils.h"
#include "..\framework\debug\DebugMacros.h"

//------------------------------------------------------------------------------
//-----------------------CREATE INI FILE PARSER---------------------------------
//------------------------------------------------------------------------------


CxIniFileParser::CxIniFileParser():
  stateFSM(ST_LOOK_SECTION_START),
  index(0)
{

}

CxIniFileParser::~CxIniFileParser()
{

}

bool CxIniFileParser::ParseBuffer(char *pBuff, unsigned long dwLen)
{   
   for(unsigned long i = 0; i < dwLen; i++)
   {
     if( true == ProcessData(pBuff[i]) ) return true;
   }
   return false;
}

bool CxIniFileParser::ProcessData(unsigned char btData)
{
   switch(stateFSM)
   {
      // Search for start of section '['
      case ST_LOOK_SECTION_START :
                if(btData == '[')
                {
                   index = 0;                                                   // reset index
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
		   section[index] = '\0';	                                // terminate command

                   if( false == isItEtalonSection() ) stateFSM = ST_LOOK_SECTION_START;
		    else { index = 0; stateFSM = ST_LOOK_KEY; }	                // goto get data state
		}
		break;

      case ST_LOOK_KEY :
      		if( btData != '=' )
                {
                   if( (btData != '\n') && (btData != '\r') )
                   {
                      if( btData == '[' ){
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
		   key[index] = '\0';	                                        // terminate command
                   index = 0;

                   if( false == isItEtalonKey() ) stateFSM = ST_LOOK_VALUE;
		    else {stateFSM = ST_LOOK_OUR_VALUE;}		        // goto get data state
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
		   value[index] = '\0';	                                        // terminate command
                   index = 0;
		   stateFSM = ST_LOOK_KEY;		                        // goto get data state
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
		   value[index] = '\0';	                                        // terminate command
		   stateFSM = ST_LOOK_SECTION_START;		                // goto get "[" state
                   return true;
		}
		break;

      default : stateFSM = ST_LOOK_SECTION_START;
   }
   return false;
}

void CxIniFileParser::SetEtalonSection( char* section )
{
  mod_memset( etalonSection, 0, SECLENGTH, SECLENGTH );
  mod_memcpy( etalonSection, section, mod_strlen(section, SECLENGTH), SECLENGTH );
}

void CxIniFileParser::SetEtalonKey( char* key )
{
  mod_memset( etalonKey, 0, KEYLENGTH, KEYLENGTH );  
  mod_memcpy( etalonKey, key, mod_strlen(key, KEYLENGTH), KEYLENGTH );
}

bool CxIniFileParser::isItEtalonSection()
{  
  if( false == mod_strcmp(section, etalonSection) ) return false;
  return true;
}

bool CxIniFileParser::isItEtalonKey()
{
  if( false == mod_strcmp(key, etalonKey) ) return false;
  return true;
}

//------------------------------------------------------------------------------


bool CxIniFileParser::ReadBool( char *IniFileName, char*section, char*id, bool defoult )
{          
   unsigned short read_length = true;
   bool result = defoult;

   int ID = FileOpen( IniFileName, FA_READ );
   
   if( -1 != ID )
   {
     SetEtalonSection(section);
     SetEtalonKey(id);

     while( 0 != read_length )
     {
       read_length = FileRead( ID, tmp_ini_buf, INI_BUF_SIZE );       
              
       if( true == ParseBuffer(tmp_ini_buf, read_length) )
       { 
         result = static_cast<bool>( atoi(GetValue()) );
         break; 
       }

     }
     
     FileClose( ID );   
   }  
     
   return result;
}


long  CxIniFileParser::ReadLong ( char *IniFileName, char*section, char*id, long defoult )
{
   unsigned short read_length = true;
   long result = defoult;
   
   int ID = FileOpen( IniFileName, FA_READ );
   
   if( -1 != ID )
   {
     SetEtalonSection(section);
     SetEtalonKey(id);

     while( 0 != read_length )
     {     
       read_length = FileRead( ID, tmp_ini_buf, INI_BUF_SIZE );
       if( true == ParseBuffer(tmp_ini_buf, read_length) )
       { 
         result = static_cast<long>( atol(GetValue()) );
         break; 
       }     
     }
     
     FileClose( ID );   
   }  

   return result;
}


float CxIniFileParser::ReadFloat( char *IniFileName, char*section, char*id, float defoult )
{
   unsigned short read_length = true;
   float result = defoult;
   
   int ID = FileOpen( IniFileName, FA_READ );
   
   if( -1 != ID )
   {
     SetEtalonSection(section);
     SetEtalonKey(id);

     while( 0 != read_length )
     {     
       read_length = FileRead( ID, tmp_ini_buf, INI_BUF_SIZE );
       if( true == ParseBuffer(tmp_ini_buf, read_length) )
       { 
         result = static_cast<float>( atof(GetValue()) );
         break; 
       }   
     }

     FileClose( ID );   
   }  
   
   return result;
}

char* CxIniFileParser::ReadString( char *IniFileName, char*section, char*id )
{
   unsigned short read_length = true;
   char* result = NULL;
   
   int ID = FileOpen( IniFileName, FA_READ );
   
   if( -1 != ID )
   {
     SetEtalonSection(section);
     SetEtalonKey(id);

     while( 0 != read_length )
     {      
       read_length = FileRead( ID, tmp_ini_buf, INI_BUF_SIZE );
       if( true == ParseBuffer(tmp_ini_buf, read_length) )
       { 
         result = GetValue();
         break; 
       }  
     }

     FileClose( ID );   
   }  
   
   return result;
}

