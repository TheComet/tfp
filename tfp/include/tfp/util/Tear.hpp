#pragma once

#include "tfp/config.hpp"
#include "tfp/listeners/TearListener.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include <stdarg.h>

namespace tfp {

class TFP_PUBLIC_API Tears
{
public:
    void cry(const char* fmt, ...);
    void vcry(const char* fmt, va_list va);

    ListenerDispatcher<TearListener> dispatcher;
};

TFP_PUBLIC_API extern Tears g_tears;

}
