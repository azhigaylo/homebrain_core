#ifndef _SLOG_H_
#define _SLOG_H_

//------------------------------------------------------------------------------
#define TRACE_SINK_CONSOLE 0
#define TRACE_SINK_DLT     1

#define TRACE_SINK TRACE_SINK_DLT
//------------------------------------------------------------------------------
/**
 * set debug level
 * @param debug level
 */
void setDbgLevel(int lvl);

/**
 * init DLT trace component
 */
void initDlt();

/**
 * deinit DLT trace component
 */
void deinitDlt();

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
