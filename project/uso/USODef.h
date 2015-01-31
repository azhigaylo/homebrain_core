#ifndef _USODEF
#define _USODEF
//---------------------------------------------------------------------
#define  RdFlash      (0x01)   //    code of read flash
#define  RdUso        (0x03)   //    code of read USO
#define  WrUso        (0x06)   //    code of write USO
#define  WrRegUso     (0x10)   //    code of write USO register's 
#define  ChAddrUso    (0x0D)   //    code of chang addres USO
#define  IdentUso     (0x00)   //    code of identification USO
//---------------------------------------------------------------------
// USO status code 
#define USO_Status_OK        0 // error not found
#define USO_Status_NoReply   1 // reply from USO is absent
#define USO_Status_COM_Err   2 // CRC or parity error
#define USO_Status_ReplyErr  3 // error in reply data

//---------------------------------------------------------------------

#define ATYPE_AI_20mA     0x01  // 4..20 mA
#define ATYPE_AI_5mA      0x02  // 0..5 mA
#define ATYPE_AI_pm10V    0x03  // +-10V
#define ATYPE_AI_01V      0x04  // 0.1V
#define ATYPE_AI_36V      0x05  // 36V
#define ATYPE_AI_120V     0x06  // ~120V
#define ATYPE_AI_D300V    0x07  // -300V
#define ATYPE_AI_TCM100   0x08  // TCM 100 
#define ATYPE_AI_A300V    0x09  // ~300V
#define ATYPE_AI_Gen      0x0A  // 
#define ATYPE_AI_Freq     0x0B  // Hz
#define ATYPE_DI3         0x0C  // 3 DI
#define ATYPE_DI2         0x0D  // 2 DI
#define ATYPE_DI1         0x0E  // 1 DI
#define ATYPE_AI_20mA_Z   0x18  // 4..20 mA ZOLOTAREV
#define ATYPE_AI_TCP100   0x19  // TCP 100 
#define ATYPE_AI_TCP100_A   0x20  // TCP 100 AGAT 

#define ATYPE_AO_Gen      0x0F  // 
#define ATYPE_AO_Linear   0x10  // 
#define ATYPE_AO_100pc    0x11  // 0..100 % 
#define ATYPE_AO_10V      0x12  // 0..10 V
#define ATYPE_AO_20mA     0x13  // 0..20 mA
#define ATYPE_DO3         0x14  // 3 DO
#define ATYPE_unuse       0x15  //
#define ATYPE_DO2         0x16  // 2 DO
#define ATYPE_DO1         0x17  // 1 DO

//------------------------------------------
#define  AI_4_20     (3.8147556e-4)        // 
#define  AI_4_20_Z   (4.1666666e-4)        // 
#define  AI_0_5      (9.53689e-5)          // 
#define  AI_10_10    (5.0863e-4)           // 
#define  AI_0_01     (1.9074E-6)           // 
#define  AI_0_36     (6.8666E-4)           // 
#define  AI_0_120    (2.2889E-3)           // 
#define  AI_DC_300   (5.7221E-3)           // 
#define  AI_TCP100   (0.003127)            // 
#define  AI_TCP100_A (0.0076248)           // 
          #define R0 ((float)(100.0))     //          
          #define A  ((float)(4.318e-3))  // TCM100
          #define B  ((float)(-5.829e-7)) //           
//          #define A  ((float)(3.9655e-3))  // TCP 100
//          #define B  ((float)(-5.829e-7))  //                    
          #define K  ((float)(-A/(2*B)))  //
          #define K2 ((float)(K*K))       //
#define  AI_AC_300  (5.7221E-3)           // 
//------------------------------------------
#define A_PARAM_UNKNOWN           0       // 
#define A_PARAM_RELIABLE          1       // 
#define A_PARAM_MORECODESP        2       // 
#define A_PARAM_LESSCODESP        3       // 
#define A_PARAM_MOREVALSP         4       // 
#define A_PARAM_LESSVALSP         5       // 
#define A_PARAM_CRASH             6       // 

//---------------------------------------------------------------------

// state code of MUK
#define MUK_Fail        0   // MUK connection error
#define MUK_OK          1   // MUK work correctly in distance mode
#define MUK_SleepMode   2   // MUK work in sleep mode
#define MUK_LocalMode   3   // MUK work in manual mode
// state code of valve
#define VALVE_Unknown   0   // valve state - unknown
#define VALVE_Close     1   // valve - close
#define VALVE_Open      2   // valve - open
#define VALVE_Middle    3   // valve in middle state
#define VALVE_Opening   4   // open process beginning
#define VALVE_Closing   5   // close process beginning
// state code of gear
#define GEAR_Unknown       0x00    // gear state - unknown
#define GEAR_OK            0x01    // gear work correctly 
                                   // subpropertis - GEAR_Opening,GEAR_Closing, GEAR_Tightening
#define GEAR_Opening       0x02    // indication "MUK opening"
#define GEAR_Closing       0x04    // indication "MUK closing"
#define GEAR_Moving        (GEAR_Opening|GEAR_Closing)
#define GEAR_Tightening    0x08    // indication "MUK tightening"                                   
// alarm gear code 
#define GEAR_SelfChgM      0x0010   // alarm - self-switching from middle mode
#define GEAR_SelfChgO      0x0020   // alarm - self-switching from open mode 
#define GEAR_SelfChgC      0x0040   // alarm - self-switching from close mode
#define GEAR_OpenJamming   0x0080   // alarm - jam in openinng process
#define GEAR_CloseJamming  0x0100   // alarm - jam in closing process
#define GEAR_Leakage       0x0200   // alarm - leakage detected
#define GEAR_ShortO        0x0400   // alarm - short circuit in opening control circuit
#define GEAR_ShortC        0x0800   // alarm - short circuit in closing control circuit
#define GEAR_ShortT        0x1000   // alarm - short circuit in tightening control circuit
#define GEAR_BreakO        0x2000   // alarm - break in opening control circuit
#define GEAR_BreakC        0x4000   // alarm - break in closing control circuit
#define GEAR_BreakT        0x8000   // alarm - break in tightening control circuit
#define GEAR_Fail          0xFFF0   // alarm - gear failure(connection - OK, data - resived)
// bit in status word
#define TAP_BreakO         0x0001   // break in opening control circuit
#define TAP_ShortO         0x0002   // short circuit in opening control circuit
#define TAP_OpenJamming    0x0004   // jam in openinng process
#define TAP_BreakC         0x0008   // break in closing control circuit
#define TAP_ShortC         0x0010   // short circuit in closing control circuit
#define TAP_CloseJamming   0x0020   // jam in closing process

#define TAP_Move           0x0100   // gear in moove process
#define TAP_SelfMove       0x0200   // gear self-switching 
#define TAP_Open           0x0400   // gear opened 
#define TAP_Close          0x0800   // gear closed
#define TAP_Handel         0x1000   // MUK work in manual mode
#define TAP_flou           0x2000   // leakage detected
// command for MUK
#define MUK_CLOSE          0x01      // MUK open 
#define MUK_OPEN           0x02      // MUK close 
#define MUK_RESET          0x03      // MUK reset
#define MUK_SHOWMODE       0x04      // show indication on front panel
#define MUK_SLEEPMODE      0x05      // put MUK in sleep mode
#define MUK_REMOTEMODE     0x06      // put MUK in distantion mode(while reset not will be)
#define MUK_TIGHTEN        0x07      // gear tightening 
#define MUK_SOFTRESET      0x08      // reset error in MUK

#endif /*_USODEF*/
