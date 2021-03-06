/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

//------------------------------------------------------------------------------

#include "interfaces/MBusDB.h"

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
