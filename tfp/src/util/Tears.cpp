#include "tfp/util/Tears.hpp"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

tfp::Tears tfp::g_tears;

using namespace tfp;

// ----------------------------------------------------------------------------
void Tears::cry(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vcry(fmt, va);
    va_end(va);
}

// ----------------------------------------------------------------------------
void Tears::vcry(const char* fmt, va_list va)
{
    // Determine length of buffer
    va_list va_len;
    va_copy(va_len, va);
    int len = vsnprintf(NULL, 0, fmt, va_len);
    va_end(va_len);

    // Allocate buffer and copy string into it
    char* msg = (char*)malloc((len + 1) * sizeof(char));
    vsprintf(msg, fmt, va);

    dispatcher.dispatch(&TearListener::onTears, msg);
    free(msg);
}
