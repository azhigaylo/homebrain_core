#ifndef _CX_INIFILE_PARSER
#define _CX_INIFILE_PARSER

//------------------------------------------------------------------------------
#include "ptypes.h"
//------------------------------------------------------------------------------
#define SECLENGTH       16   // max value for segment
#define KEYLENGTH       16   // max value for key length
#define VALUELENGTH     40   // max value for paramiter
#define INI_BUF_SIZE    20   // size of buffer for parsing cfg fule
//------------------------------------------------------------------------------
enum ST_INI_PROCESS 
{
   ST_LOOK_SECTION_START = 0,   // Search for start section
   ST_LOOK_SECTION_END,         // Search for end section
   ST_LOOK_KEY,                 // Get key
   ST_LOOK_VALUE,               // Get every paramiter
   ST_LOOK_OUR_VALUE            // Get last paramiter
};
//------------------------------------------------------------------------------

class CxIniFileParser
{
  public:
    CxIniFileParser();
     ~CxIniFileParser();

    bool    ReadBool  ( const char *IniFileName, const char*section, const char*id, bool  defoult );
    int32_t ReadInt   ( const char *IniFileName, const char*section, const char*id, int32_t defoult );
    float   ReadFloat ( const char *IniFileName, const char*section, const char*id, float defoult );
    char*   ReadString( const char *IniFileName, const char*section, const char*id );

  private:

    bool ParseBuffer(const char *pBuff, uint32_t dwLen);
    char* GetValue(){return value;}
    void SetEtalonSection( const char* section );
    void SetEtalonKey( const char* key );

    bool ProcessData(uint8_t btData);
    bool isItEtalonSection();
    bool isItEtalonKey();

    int8_t section[SECLENGTH];
    int8_t key[KEYLENGTH];
    int8_t value[VALUELENGTH];
    int8_t index;

    int8_t etalonSection[SECLENGTH];
    int8_t etalonKey[KEYLENGTH];

    ST_INI_PROCESS stateFSM;              // Current state parser is in

    int8_t tmp_ini_buf[INI_BUF_SIZE];

}; typedef CxIniFileParser *pTCxIniFileParser;

#endif /*_CX_INIFILE_PARSER*/
