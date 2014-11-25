#include <string.h>
#include <math.h>
#include <stdarg.h>
#include "utils.h"
//#include "CxAssertions.h"

//-----------------------------Utilities-Routines-------------------------------

//word from byte
unsigned short GenWfrom2B(char B_h, char B_l)
{
  return( ((unsigned short)B_h<<8)|((unsigned short)(B_l)) );  
}

//Long from word
unsigned long GenLfrom2W(unsigned short W_h, unsigned short W_l)
{
  return(((unsigned long)W_h<<16)|((unsigned long)W_l));  
}

// function return LSB of word           
unsigned char LOW(unsigned short celoe)
{   
  return((char)(0x00FF & celoe));
}

// function return MSB of word           
unsigned char HIGH(unsigned short celoe)
{  
  return((char)(celoe>>8));  
} 

//turn bit in byte
char TurnBitInByte(char Byte)
{
   unsigned char rez=0;
   for(unsigned char i=0;i<8;i++){
     if((unsigned char)(0x80>>i) & Byte)rez |= (unsigned char)((0x01)<<i);      
   } 
   return(rez);  
}
 
void LShiftArray(char*buff,unsigned short b_size,unsigned short shift)
{ 
  if(b_size > shift){
    unsigned short shiftsize = b_size-shift;
    for(unsigned short i=0; i<shiftsize; i++){
      *(buff+i) = *(buff+shift+i);  
    }
    memset(buff+shiftsize,0x33,shift);      
  }else{
    memset(buff,0x33,b_size);
    //ASSERTION(e_lshift);   
  }  
}
 
void revertbuff( char*dest,unsigned short chng_size )
{
   char tmp_d = 0; 
   unsigned short custom_counter = 0;
   if(chng_size)chng_size--;
   while(custom_counter<chng_size)
   {
     tmp_d = dest[custom_counter];
     dest[custom_counter++] = dest[chng_size];
     dest[chng_size--] = tmp_d;     
   }  
}

//-----------------------------ModBus utylites----------------------------------

//function convert MB short
unsigned short getWordFromMbReg( unsigned short registerMB )
{
   return((registerMB>>8)|(registerMB<<8));
} 

long getLongFromTwoMbReg( unsigned short registerMB_1, unsigned short registerMB_2 )
{
  long result = (getWordFromMbReg(registerMB_2)<<16) | (getWordFromMbReg(registerMB_1));
  return result;   
}

float getFloatFromTwoMbReg( unsigned short registerMB_1, unsigned short registerMB_2 )
{
  union { float result; long temp;}tmpBlock;
  tmpBlock.temp = (getWordFromMbReg(registerMB_2)<<16) | (getWordFromMbReg(registerMB_1));
  return tmpBlock.result;
}

//-----------------------------Calculation CRC----------------------------------

// CRC table
static const unsigned char CRC_Hi[] = {
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
};

static const unsigned char CRC_Lo[] = {
 0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
 0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
 0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
 0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
 0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
 0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
 0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
 0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
 0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
 0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
 0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
 0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
 0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
 0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
 0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
 0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40,
};

unsigned short CRC16b(unsigned char *Buff, unsigned short Count, unsigned char base)
{
  unsigned char ah = base;
  unsigned char al = base;
  unsigned char bl;
  
  if(Count<3)return (0x000F);
  while (Count) {
    bl = *Buff ^ ah;
    ah = al ^ CRC_Hi[bl];
    al = CRC_Lo[bl];
    Buff++; Count--;
  }
  return (unsigned short)(al<<8) | ah;
}

unsigned short CRC16_T(unsigned char *Buff, unsigned short Count)
{
  return CRC16b(Buff, Count, 0xFF);
}

unsigned short  CRC16_T_0(unsigned char *Buff, unsigned short Count)
{
  return CRC16b(Buff, Count, 0);
}
//-----------------------------
/*
unsigned short CRC16_T(unsigned char *Buff, unsigned short Count, unsigned short in_crc)
{
  unsigned char al = HIGH(in_crc);
  unsigned char ah = LOW(in_crc);
  unsigned char bl;
  
  if(Count<3)return (0x000F);
  while (Count) {
    bl = *Buff ^ ah;
    ah = al ^ CRC_Hi[bl];
    al = CRC_Lo[bl];
    Buff++; Count--;
  }
  return (unsigned short)(al<<8) | ah;
}
*/
//-----------------------------standard functions which had been modified-------

bool mod_memcpy( char*to, char*from, unsigned short datasize, unsigned short sizeofto )
{
  if(sizeofto >= datasize) {memcpy(to, from, datasize); return true;}
  else{
    //ASSERTION(e_memcpy);
    memcpy(to, from, sizeofto); 
    return false;
  }
}

bool mod_memset( char*to, char simb, unsigned short datasize, unsigned short sizeofto )
{
  if(sizeofto >= datasize) {memset(to, simb, datasize); return true;}
  else{
    //ASSERTION(e_memset);
    memset(to, simb, sizeofto); 
    return false;
  }
}

unsigned short mod_strlen( char*string, unsigned short reasonvalue )
{
  unsigned short len = strlen(string);
  if(len > reasonvalue) len = reasonvalue;
  return(len);
}

bool mod_strstr( char*string1, char*string2, unsigned short len1, unsigned short len2 )
{
  return strstr(string1, string2);
}

char* mod_strchr( char* string1, char symbol, unsigned short sStringSize )
{
  char* tmp_out = strchr( string1, symbol );
  if( tmp_out > (string1 + sStringSize ) ) tmp_out = NULL;
  
  return tmp_out;
}

char* mod_strrchr( char* string1, char symbol, unsigned short sStringSize )
{
  char* tmp_out = strrchr( string1, symbol );
  if( tmp_out > (string1 + sStringSize ) ) tmp_out = NULL;
  
  return tmp_out;
}

bool mod_strncpy( char* e_str, char* s_str, unsigned short sizeofto )
{
  unsigned short n_term = mod_strlen( s_str, sizeofto );
  return mod_memcpy( e_str, s_str, n_term, sizeofto );
}

bool mod_strcmp( char *s1, char *s2 )    // true - if equil
{
  bool result = false;
  if( strcmp(s1,s2) == 0 ) result = true;
  return result;
}

int mod_sprintf_new( char *e_str, unsigned short dest_size, const char* pFormat, va_list *dataList )
{ 
  const char *sofs = pFormat;
  short pointr_in_buf = 0;

  va_list argPtr = *dataList;

  while( (*sofs != 0) && (pointr_in_buf < dest_size) )
  {       
       char symbol = *sofs++;
        
       if( symbol == '%')
       {               
         //content analisis
 	 symbol = *sofs++;

	 switch( symbol )
         {

		case  0  : {break;}
		case 's' : {
                              char *string_strt = va_arg( argPtr, char * );
                              
                              short s_counter = 0;
                              char data = string_strt[s_counter++];

                              while( (data != 0) &&  (pointr_in_buf <= dest_size) ) 
                              {                               
                                e_str[pointr_in_buf++] = data;                             
                                data = string_strt[s_counter++];  
                              }   
                              break;
                           }                
		case 'b' : {
                              char data = va_arg( argPtr, int );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf );  
                              break;
                           }
		case 'w' : {
                              short data = va_arg( argPtr, int );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf ); 
                              break;
                           }
                case 'l' : {
                              long data = va_arg( argPtr, int );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf ); 
                              break;
                           }
		case '.' : {
                              char dig_after_point = (*sofs++)-0x30;
                              if( (*sofs++ == 'f') && (dig_after_point < 9 ) )
                              {
                                float data = va_arg( argPtr, double );
                                pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dig_after_point, dest_size-pointr_in_buf );                               
                              }
                              break;
                           }
		default	 : {
                              break;
                           }
	 }	
	}
        else
        {
	  // put symbol in buffer in position pointr_in_buf
	  e_str[pointr_in_buf++] = symbol;
	}       
  }

  return pointr_in_buf;
}

int mod_sprintf( char *e_str, unsigned short dest_size, char *format, void*ptr1,  
                                     void*ptr2, void*ptr3, void*ptr4, void*ptr5, void*ptr6 )
{ 
  char *sofs = format;
  short pointr_in_buf = 0;
  char total_counter = 1;
  void *ptr_on_paramiter = NULL;
  
  while( (*sofs != 0) && (pointr_in_buf < dest_size) )
  {       
       char symbol = *sofs++;
        
       if( symbol == '%')
       {
         
         // calculation of location data in the stack with the possibility 
         // of finding elements in the registers !!! only for ARV      
         switch( total_counter++ ){
           case  1 : { ptr_on_paramiter = ptr1; break; } 
           case  2 : { ptr_on_paramiter = ptr2; break; } 
           case  3 : { ptr_on_paramiter = ptr3; break; } 
           case  4 : { ptr_on_paramiter = ptr4; break; } 
           case  5 : { ptr_on_paramiter = ptr5; break; } 
           case  6 : { ptr_on_paramiter = ptr6; break; }  
           default : { break; }
         }  
         // finished calculation of parameters location 
                           
         //content analisis
 	 symbol = *sofs++;

	 switch( symbol )
         {

		case  0  : {break;}
		case 's' : {
                              char *string_strt = reinterpret_cast<char*>(ptr_on_paramiter);
                              
                              short s_counter = 0;
                              char data = string_strt[s_counter++];

                              while( (data != 0) &&  (pointr_in_buf <= dest_size) ) 
                              {                               
                                e_str[pointr_in_buf++] = data;                             
                                data = string_strt[s_counter++];  
                              }   
                              break;
                           }
		case 'b' : {
                              char data = *( reinterpret_cast<char*>(ptr_on_paramiter) );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf );  
                              break;
                           }
		case 'w' : {
                              short data = *( reinterpret_cast<short*>(ptr_on_paramiter) );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf ); 
                              break;
                           }
                case 'l' : {
                              long data = *( reinterpret_cast<long*>(ptr_on_paramiter) );
                              pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dest_size-pointr_in_buf ); 
                              break;
                           }
		case '.' : {
                              char dig_after_point = (*sofs++)-0x30;
                              if( (*sofs++ == 'f') && (dig_after_point < 9 ) )
                              {
                                float data = *( reinterpret_cast<float*>(ptr_on_paramiter) );
                                pointr_in_buf += printinbuff( (e_str+pointr_in_buf), data, dig_after_point, dest_size-pointr_in_buf );                               
                              }
                              break;
                           }
		
		default	 : {
                              break;
                           }
	 }	
	}
        else
        {
	  // put symbol in buffer in position pointr_in_buf
	  e_str[pointr_in_buf++] = symbol;
	}
       
  }
  
  return pointr_in_buf;
}

//------------------------basic function for print in buffer--------------------

unsigned short printinbuff( char* dest,char* src,unsigned short dest_size )
{
  unsigned short str_size = 0; 
  if(dest && src && dest_size){
    str_size = (unsigned short)strlen(src);      
    mod_memcpy(dest,src,str_size,dest_size);               
  }else{ /*ASSERTION(e_sprnbuf);*/ }
  return str_size; 
}

unsigned short printinbuff( char*dest,char value,unsigned short dest_size )
{
  unsigned short custom_counter = 0;
  if(dest && dest_size){
    do{
      dest[custom_counter++] = value%10 + 0x30;
      value /=10;             
    }while(value && (dest_size > custom_counter));    
    revertbuff(dest,custom_counter);
  }else{ /*ASSERTION(e_cprnbuf);*/ } 
  return custom_counter;
}

unsigned short printinbuff( char*dest,unsigned short value,unsigned short dest_size )
{
  unsigned short custom_counter = 0;
  if(dest && dest_size){  
    do{
      dest[custom_counter++] = value%10 + 0x30;
      value /=10;             
    }while(value && (dest_size > custom_counter));    
    revertbuff(dest,custom_counter);
  }else{ /*ASSERTION(e_iprnbuf);*/ }  
  return custom_counter;
}

unsigned short printinbuff( char*dest,unsigned long value,unsigned short dest_size )
{
  unsigned short custom_counter = 0;
  if(dest && dest_size){  
    do{
      dest[custom_counter++] = value%10 + 0x30;
      value /=10;             
    }while(value && (dest_size > custom_counter));    
    revertbuff(dest,custom_counter);
  }else{ /*ASSERTION(e_lprnbuf);*/ }  
  return custom_counter;
}

//------------------------derivative functions for the print in a buffer--------

unsigned short printinbuff( char*dest, short value,unsigned short dest_size )
{
  unsigned short custom_counter = 0;
  if( (value >= 0) && (dest_size > 0) ){
    custom_counter = printinbuff( dest, (unsigned short)value, dest_size );
  }else{    
    value *=-1;
    dest[custom_counter++] = '-';
    custom_counter += printinbuff( dest+custom_counter, value, dest_size-custom_counter );
  }  
  return custom_counter;
}

unsigned short printinbuff( char*dest, long value, unsigned short dest_size )
{
  unsigned short custom_counter = 0;
  if( (value >= 0) && (dest_size > 0) ){
    custom_counter = printinbuff( dest, (unsigned long)value, dest_size );
  }else{    
    value *=-1;
    dest[custom_counter++] = '-';
    custom_counter += printinbuff( dest+custom_counter, value, dest_size-custom_counter );
  }  
  return custom_counter;
}

unsigned short printinbuff( char*dest,float value, char dig_after_point, unsigned short dest_size )
{
  unsigned short custom_counter = 0;

  long main_part = static_cast<long>(value);
  unsigned long after_point_part = 0;
  
  if( main_part >= 0 ){    
    after_point_part = static_cast<long>( (value - static_cast<float>(main_part)) * pow((double)10, (int)dig_after_point) );
  }else{  
    after_point_part = static_cast<long>( (static_cast<float>(main_part) - value) * pow((double)10, (int)dig_after_point) );  
  }  
  
  custom_counter = printinbuff( dest, main_part, dest_size );
  if( (dig_after_point > 0) && ((dest_size - custom_counter) > 0) )
  {
    dest[custom_counter++] = '.';
    custom_counter += printinbuff( dest+custom_counter, after_point_part, dest_size - custom_counter );
  }
  
  return custom_counter;
}

unsigned short checkdest( unsigned short &dest_size, unsigned short &busy_size )
{
    if(dest_size > busy_size)return(dest_size-busy_size); 
    return 0;
}