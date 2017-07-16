#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Type.hpp"
#include "kiss_fft/kiss_fft.h"

#include <string>
#include <cstdlib>

#ifdef TFP_PLATFORM_WINDOWS
#   define isnan(x) _isnan(x)
#   define isinf(x) (!_finite(x))
#   define fpu_error(x) (isinf(x) || isnan(x))
#else
#   define fpu_error(x) (std::isinf(x) || std::isnan(x))
#endif

// We require an overload of atof() for std::string. It just forwards c_str()
namespace std {
    double atof(const std::string& str);
}

namespace tfp {

extern const double pi;

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#if !HAVE_ASINH
double asinh(const double d);
Type<Real>::Complex asinh(const Type<Real>::Complex& c);
#endif
#if !HAVE_ACOSH
double acosh(const double d);
Type<Real>::Complex acosh(const Type<Real>::Complex& c);
#endif
#if !HAVE_ATANH
double atanh(const double d);
Type<Real>::Complex atanh(const Type<Real>::Complex& c);
#endif

void complexMagnitude(double* magnitude, const kiss_fft_cpx* transformedSignal, unsigned len);
void complexPhase(double* phase, const kiss_fft_cpx* transformedSignal, unsigned len);
void copyDoublesIntoCpxBuffer(kiss_fft_cpx* target, const double* src, unsigned len);
void multiplyCpxBuffer(kiss_fft_cpx* target, const double product, unsigned len);

template <class T, class U>
void copyBuffer(T* target, const U* src, unsigned len)
{
    while(len--)
        *target++ = *src++;
}

} // namespace tfp
