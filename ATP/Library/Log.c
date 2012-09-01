#include "Log.h"

#include <stdarg.h>

void ATP_log(const char *p_format, ...)
{
    va_list l_list;

    va_start(l_list, p_format);
    vprintf(p_format, l_list);
    va_end(l_list);
}

void ATP_logErr(const char *p_file, unsigned int p_line, const char *p_format, ...)
{
    va_list l_list;
    char l_fmt[1024];

    snprintf(l_fmt, sizeof(l_fmt), "ERROR [%s:%u]: %s", p_file, p_line, p_format);

    va_start(l_list, p_format);
    vfprintf(stderr, l_fmt, l_list);
    va_end(l_list);
}

void ATP_logDbg(const char *p_file, unsigned int p_line, const char *p_format, ...)
{
    va_list l_list;
    char l_fmt[1024];

    snprintf(l_fmt, sizeof(l_fmt), "DEBUG [%s:%u]: %s", p_file, p_line, p_format);

    va_start(l_list, p_format);
    vprintf(l_fmt, l_list);
    va_end(l_list);
}
