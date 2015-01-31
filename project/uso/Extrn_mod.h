//--------------------in this part you must wrote USO, global var........---------------------

extern CUART_BASE_ptr pCOM1;   
extern CUART_BASE_ptr pCOM2;   
extern CUART_BASE_ptr pCOM3;   
extern CUART_BASE_ptr pCOM4; 
/*  
//--------------------AI MODULE-----------------------
//TAI_USO  MA16_1(port,address,number of chanel,pointer on chanell array,Dpoint for module status);
// MA-16
TAioChannel AI1_CH[16] = {
 {  0, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 10},    //
 {  1, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 11},    // 
 {  2, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 12},    // 
 {  3, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 13},    //  
 {  4, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 14},    //
 {  5, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 15},    //
 {  6, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 16},    //
 {  7, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 17},    // 
 {  8, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 18},    //
 {  9, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 19},    //
 { 10, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 20},    //
 { 11, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 21},    // 
 { 12, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 22},    // 
 { 13, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 23},    //   
 { 14, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 24},    //
 { 15, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 25}     // 
}; 
TAI_USO  MA16_1(pCOM2,0x01,16,AI1_CH,10);

// MA-8
TAioChannel AI2_CH[8] = {
 {  0, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 30},    //
 {  1, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 31},    // 
 {  2, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 32},    // 
 {  3, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 33},    //
 {  4, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 34},    //
 {  5, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 35},    // 
 {  6, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 36},    //
 {  7, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 37},    //  
}; 
TAI_USO  MA8_1(pCOM2,0x02,8,AI2_CH,11);

// MA-4
TAioChannel AI3_CH[16] = {
 {  0, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 26},    //
 {  1, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 27},    // 
 {  2, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 28},    // 
 {  3, ATYPE_AI_20mA,  0x0004, 0x0014, 0x00, 0x0032, 0, 0, 0, 0, 29}     //
}; 
TAI_USO  MA4_1(pCOM2,0x03,4,AI3_CH,12);

//--------------------AO MODULE-----------------------

//--------------------DI MODULE-----------------------

//--------------------DO MODULE-----------------------

//--------------------MUK MODULE----------------------
//TAI_USO  MA16_1(port,address,Dpoint for module status, Dpoint for start info block, Dpoint for start command block);
TMUK_USO MUK_1(pCOM2,0x04,13,0,40);
*/
