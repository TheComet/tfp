#include "sfgsym/util/log.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
int
sfgsym_log(const char* fmt, ...)
{
    int len;
    va_list ap;
    va_start(ap, fmt);
    len = vprintf(fmt, ap);
    va_end(ap);
    return len;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_vlog_error(const char* fmt, va_list ap)
{
    return vprintf(fmt, ap);
}
