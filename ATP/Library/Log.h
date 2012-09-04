/* File: Log.h
Simple logging functionality for ATP.
*/
#ifndef _ATP_LIBRARY_LOG_H_
#define _ATP_LIBRARY_LOG_H_

#include "Export.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Macro: LOG
Log a formatted message to stdout.
*/
#define LOG(...)    ATP_log(__VA_ARGS__)
/* Macro: ERR
Log a formatted error message to stderr.  The location of the error will be prepended to the message.
*/
#define ERR(...)    ATP_logErr(__FILE__, __LINE__, __VA_ARGS__)
/* Macro: PERR
Log the current result of strerror(errno) to stderr using <ERR>.
*/
#define PERR()      ERR("%s", strerror(errno))

/* Macro: DBG
Log a formatted debug message to stdout.  The location of the message will be prepended to the message.
Debug messages will only be printed in debug builds.
*/
#ifdef NDEBUG
    #define DBG(...)
#else
    #define DBG(...)    ATP_logDbg(__FILE__, __LINE__, __VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* Function: ATP_log
Log a message to stdout.

Parameters:
    p_format - The message format string.
    ...      - The format parameters.
*/
EXPORT void ATP_log(const char *p_format, ...);
/* Function: ATP_logErr
Log an error message to stderr.

Parameters:
    p_file   - The source file where the error occured.
    p_line   - The source line number of the error.
    p_format - The message format string.
    ...      - The format parameters.
*/
EXPORT void ATP_logErr(const char *p_file, unsigned int p_line, const char *p_format, ...);
/* Function: ATP_logDbg
Log a debug message to stout.

Parameters:
    p_file   - The source file of the message.
    p_line   - The source line number of the message.
    p_format - The message format string.
    ...      - The format parameters.
*/
EXPORT void ATP_logDbg(const char *p_file, unsigned int p_line, const char *p_format, ...);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* _ATP_LIBRARY_LOG_H_ */
