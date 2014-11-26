#ifndef _COMMON_H_
#define _COMMON_H_

/**********************************************************************
*  Project       Harman Car Multimedia System
*  (c) copyright 2012
*  Company       Harman/Becker Automotive Systems GmbH
*                All rights reserved
*  Secrecy Level STRICTLY CONFIDENTIAL
**********************************************************************/

#include <inttypes.h>
#include <sys/neutrino.h>
#include <sys/trace.h>

#ifdef __cplusplus
   extern "C" {
#endif

/**
 * number of -v given as parameter
 */
extern int32_t optv;
extern uint8_t slogSeverity;


/**
 * print error
 * @param *fmt format string
 */
void printError(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printInfo(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printWarning(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printDebug(const char *fmt, ...);

#ifdef __cplusplus
   }
#endif

#endif // _COMMON_H_
