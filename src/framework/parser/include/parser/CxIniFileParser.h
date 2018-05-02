#ifndef _CX_INIFILE_PARSER
#define _CX_INIFILE_PARSER

//------------------------------------------------------------------------------
#include "common/ptypes.h"
//------------------------------------------------------------------------------
#define SECLENGTH       50   // max value for segment
#define KEYLENGTH       50   // max value for key length
#define VALUELENGTH     100  // max value for paramiter
#define INI_BUF_SIZE    50   // size of buffer for parsing cfg fule
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

    bool    ReadBool  ( const char *IniFileName, const char*sect, const char*id, bool  defoult );
    int32_t ReadInt   ( const char *IniFileName, const char*sect, const char*id, int32_t defoult );
    float   ReadFloat ( const char *IniFileName, const char*sect, const char*id, float defoult );
    char*   ReadString( const char *IniFileName, const char*sect, const char*id );

  private:

    bool ParseBuffer(const char *pBuff, int32_t dwLen);
    char* GetValue(){return value;}
    void SetEtalonSection( const char* sect );
    void SetEtalonKey( const char* k );

    bool ProcessData(uint8_t btData);
    bool isItEtalonSection();
    bool isItEtalonKey();

    char section[SECLENGTH];
    char key[KEYLENGTH];
    char value[VALUELENGTH];
    int8_t index;

    char etalonSection[SECLENGTH];
    char etalonKey[KEYLENGTH];

    ST_INI_PROCESS stateFSM;              // Current state parser is in

    char tmp_ini_buf[INI_BUF_SIZE];

}; typedef CxIniFileParser *pTCxIniFileParser;

#endif /*_CX_INIFILE_PARSER*/
