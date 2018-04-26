//------------------------------------------------------------------------------

#include "MBusDB.h"

//------------------------------------------------------------------------------

//function convert MB short

uint16_t ConvertMBint(uint16_t ReadFromMB)
{
   uint8_t tmp=0;
   
   union
   {
     struct{uint8_t one; uint8_t two;} VitrualLong;
     uint16_t fulllong;
   }ConvertStruct;

   ConvertStruct.fulllong = ReadFromMB;
   tmp = ConvertStruct.VitrualLong.one;
   ConvertStruct.VitrualLong.one = ConvertStruct.VitrualLong.two;
   ConvertStruct.VitrualLong.two = tmp;
   return(ConvertStruct.fulllong);
}
