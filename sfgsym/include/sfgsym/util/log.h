#pragma once

#include "sfgsym/config.h"
#include <stdarg.h>

C_BEGIN

SFGSYM_PUBLIC_API int
sfgsym_log(const char* fmt, ...);

SFGSYM_PUBLIC_API int
sfgsym_vlog_error(const char* fmt, va_list ap);

C_END
