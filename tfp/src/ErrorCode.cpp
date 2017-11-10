#include "tfp/ErrorCode.hpp"
#include <stddef.h>
#include <assert.h>

using namespace tfp;

static const char* g_descriptions[] = {
#define X(name, desc) "name" ": " desc,
    ERROR_LIST
#undef X
};

const char* ErrorCode::error() const
{
    if (error_ <= RET_TRUE)
        return NULL;
    return g_descriptions[error_];
}

ErrorCode::operator bool() const
{
    assert(error() == NULL);
    return error_ == RET_TRUE;
}

ErrorCode& ErrorCode::operator|=(const ErrorCode& other)
{
    if (error())
        return *this;
    if (other.error())
    {
        *this = other;
        return *this;
    }
    if (other)
        error_ = RET_TRUE;
    return *this;
}

ErrorCode& ErrorCode::operator&=(const ErrorCode& other)
{
    if (error())
        return *this;
    if (other.error())
    {
        *this = other;
        return *this;
    }
    if (!other)
        error_ = RET_FALSE;
    return *this;
}
