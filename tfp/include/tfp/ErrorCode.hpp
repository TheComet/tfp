#pragma once

#include "tfp/config.hpp"

namespace tfp {

#define ERROR_LIST \
    X(RET_FALSE, "No error. Return is false.") \
    X(RET_TRUE, "No error. Return is true.") \
    X(ERR_VAR_CYCLIC_DEPENDENCY, "Variable values must not have cyclic dependencies. Cannot substitute variable values.") \
    X(ERR_NON_CONSTANT_EXPONENT, "Variable must be raised to a constant exponent.")

enum ErrorCodeEnum
{
#define X(name, desc) name,
    ERROR_LIST
#undef X
};

struct ErrorCode
{
    ErrorCode() : error_(RET_FALSE) {}
    ErrorCode(const ErrorCode& other) : error_(other.error_) {}
    ErrorCode(ErrorCodeEnum error) : error_(error) {}
    ErrorCode(bool value) { error_ = value ? RET_TRUE : RET_FALSE; }
    
    const char* error() const;
    
    operator bool() const;
    ErrorCode& operator|=(const ErrorCode& other);
    ErrorCode& operator&=(const ErrorCode& other);

private:
    ErrorCodeEnum error_;
};

inline ErrorCode operator|(ErrorCode lhs, const ErrorCode& rhs) { return lhs |= rhs; }
inline ErrorCode operator&(ErrorCode lhs, const ErrorCode& rhs) { return lhs &= rhs; }
inline ErrorCode operator|(bool lhs, const ErrorCode& rhs) { return ErrorCode(lhs) | rhs; }
inline ErrorCode operator&(bool lhs, const ErrorCode& rhs) { return ErrorCode(lhs) & rhs; }
inline ErrorCode operator|(const ErrorCode& lhs, bool rhs) { return lhs | ErrorCode(rhs); }
inline ErrorCode operator&(const ErrorCode& lhs, bool rhs) { return lhs & ErrorCode(rhs); }

}
