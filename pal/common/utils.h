#ifndef _UTILS
#define _UTILS

#include <stdarg.h>

//-----------------------------Utilities-Routines-------------------------------

bool            mod_memcpy  ( char*to, char*from, unsigned short datasize, unsigned short sizeofto );
bool            mod_memset  ( char*to, char simb, unsigned short datasize, unsigned short sizeofto );
unsigned short  mod_strlen  ( char*string, unsigned short reasonvalue );
bool            mod_strstr  ( char*string1, char*string2, unsigned short len1, unsigned short len2 );
char*           mod_strchr  ( char* string1, char symbol, unsigned short sStringSize );
char*           mod_strrchr ( char* string1, char symbol, unsigned short sStringSize );
bool            mod_strncpy ( char* e_str, char* s_str, unsigned short sizeofto );
bool            mod_strcmp  ( char *s1, char *s2);
int             mod_sprintf_new( char *e_str, unsigned short dest_size, const char* pFormat, va_list *dataList );
int             mod_sprintf ( char *e_str, unsigned short dest_size, char *format, void*ptr1=0, void*ptr2=0, 
                                                      void*ptr3=0, void*ptr4=0, void*ptr5=0, void*ptr6=0 );

unsigned short  GenWfrom2B  ( char B_h, char B_l );
unsigned long   GenLfrom2W  ( unsigned short W_h, unsigned short W_l );
unsigned char   LOW         ( unsigned short celoe );
unsigned char   HIGH        ( unsigned short celoe );
char            TurnBitInByte( char Byte );
void            LShiftArray ( char*buff,unsigned short b_size,unsigned short shift );
void            revertbuff  ( char*dest,unsigned short chng_size );
void            inversbuff  ( char*dest,unsigned short chng_size );

unsigned short  CRC16b      ( unsigned char *Buff, unsigned short Count, unsigned char base );
unsigned short  CRC16_T     ( unsigned char *Buff, unsigned short Count );
//unsigned short  CRC16_T     ( unsigned char *Buff, unsigned short Count, unsigned short in_crc );
unsigned short  CRC16_T_0   ( unsigned char *Buff, unsigned short Count );
//-----------------------------ModBus utylites----------------------------------
unsigned short  getWordFromMbReg   ( unsigned short ReadFromMB );
float getFloatFromTwoMbReg( unsigned short registerMB_1, unsigned short registerMB_2 );

unsigned short  printinbuff ( char*dest, char*src,unsigned short dest_size );
unsigned short  printinbuff ( char*dest,char value,unsigned short dest_size );
unsigned short  printinbuff ( char*dest,unsigned short value,unsigned short dest_size );
unsigned short  printinbuff ( char*dest, short value,unsigned short dest_size );
unsigned short  printinbuff ( char*dest,unsigned long value,unsigned short dest_size );
unsigned short  printinbuff ( char*dest,long value,unsigned short dest_size );
unsigned short  printinbuff ( char*dest,float value, char dig_after_point, unsigned short dest_size );
unsigned short  checkdest   ( unsigned short &dest_size, unsigned short &busy_size );



//-------------templates--------------------------------------------------------

template <class t_var>
unsigned short printinbuff(char*dest, char*src, t_var var1, unsigned short dest_size)
{
  unsigned short busy = 0, custom_counter = 0;
  if(dest && src){
    custom_counter += busy = printinbuff(dest,src,dest_size);    
    dest_size = checkdest(dest_size, busy);    
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);
    dest_size = checkdest(dest_size, busy);  
    custom_counter += busy = printinbuff(dest+custom_counter,var1,dest_size);  
  }
  return custom_counter;
}

template <class t_var>
unsigned short printinbuff(char*dest, char*src, t_var var1, t_var var2, unsigned short dest_size)
{
  unsigned short busy = 0, custom_counter = 0;
  if(dest && src){
    custom_counter += busy = printinbuff(dest,src,dest_size);    
    dest_size = checkdest(dest_size, busy);    
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);
    dest_size = checkdest(dest_size, busy);  
    custom_counter += busy = printinbuff(dest+custom_counter,var1,dest_size);  
    dest_size = checkdest(dest_size, busy);      
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var2,dest_size);  
  }
  return custom_counter;
}


template <class t_var>
unsigned short printinbuff(char*dest, char*src, t_var var1, t_var var2, t_var var3, unsigned short dest_size)
{
  unsigned short busy = 0, custom_counter = 0;
  if(dest && src){
    custom_counter += busy = printinbuff(dest,src,dest_size);    
    dest_size = checkdest(dest_size, busy);    
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);
    dest_size = checkdest(dest_size, busy);  
    custom_counter += busy = printinbuff(dest+custom_counter,var1,dest_size);  
    dest_size = checkdest(dest_size, busy);      
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var2,dest_size);  
    dest_size = checkdest(dest_size, busy);          
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var3,dest_size);    
  }
  return custom_counter;
}

template <class t_var>
unsigned short printinbuff(char*dest, char*src, t_var var1, t_var var2, t_var var3, t_var var4, unsigned short dest_size)
{
  unsigned short busy = 0, custom_counter = 0;
  if(dest && src){
    custom_counter += busy = printinbuff(dest,src,dest_size);    
    dest_size = checkdest(dest_size, busy);    
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);
    dest_size = checkdest(dest_size, busy);  
    custom_counter += busy = printinbuff(dest+custom_counter,var1,dest_size);  
    dest_size = checkdest(dest_size, busy);      
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var2,dest_size);  
    dest_size = checkdest(dest_size, busy);          
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var3,dest_size);  
    dest_size = checkdest(dest_size, busy);              
    custom_counter += busy = printinbuff(dest+custom_counter,"\t",dest_size);    
    dest_size = checkdest(dest_size, busy); 
    custom_counter += busy = printinbuff(dest+custom_counter,var4,dest_size);  
  }
  return custom_counter;
}

#endif /*_UTILS*/
