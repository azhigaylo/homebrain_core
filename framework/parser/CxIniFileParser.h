#ifndef _CX_INIFILE_PARSER
#define _CX_INIFILE_PARSER

#define SECLENGTH       16					                // max value for segment
#define KEYLENGTH       16					                // max value for key length
#define VALUELENGTH     40				                        // max value for paramiter
#define INI_BUF_SIZE    20                                                      // size of buffer for parsing cfg fule

enum ST_INI_PROCESS 
{
	ST_LOOK_SECTION_START = 0,   		                                // Search for start section
        ST_LOOK_SECTION_END,                                                    // Search for end section
	ST_LOOK_KEY,					                        // Get key
	ST_LOOK_VALUE,					                        // Get every paramiter
        ST_LOOK_OUR_VALUE				                        // Get last paramiter
};

class CxIniFileParser{
  public:
    CxIniFileParser();    
     ~CxIniFileParser();
    
    bool  ReadBool  ( char *IniFileName, char*section, char*id, bool  defoult );
    long  ReadLong  ( char *IniFileName, char*section, char*id, long  defoult );
    float ReadFloat ( char *IniFileName, char*section, char*id, float defoult );
    char* ReadString( char *IniFileName, char*section, char*id );

  private:
   
    bool ParseBuffer(char *pBuff, unsigned long dwLen);
    char* GetValue(){return value;}
    void SetEtalonSection( char* section );
    void SetEtalonKey( char* key );
    
    bool ProcessData(unsigned char btData);
    bool isItEtalonSection();
    bool isItEtalonKey();

    char section[SECLENGTH];
    char key[KEYLENGTH];
    char value[VALUELENGTH];
    char index;

    char etalonSection[SECLENGTH];
    char etalonKey[KEYLENGTH];

    ST_INI_PROCESS stateFSM;				// Current state parser is in
    
    char tmp_ini_buf[INI_BUF_SIZE];
    
}; typedef CxIniFileParser *pTCxIniFileParser;

#endif /*_CX_INIFILE_PARSER*/
