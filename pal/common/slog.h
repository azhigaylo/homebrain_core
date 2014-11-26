#ifndef _SLOG_H_
#define _SLOG_H_

/**
 * print error
 * @param *fmt format string
 */
void printError(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printWarning(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printDebug(const char * const fmt, ...);

#endif // _SLOG_H_
