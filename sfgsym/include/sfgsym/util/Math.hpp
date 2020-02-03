#pragma once

#include "sfgsym/config.hpp"
#include "kiss_fft/kiss_fft.h"

#include <string>
#include <cstdlib>

#define fpu_error(x) \
        (std::isinf(x) || std::isnan(x))

// We require an overload of atof() for std::string. It just forwards c_str()
namespace std {
    double atof(const std::string& str);
}

namespace sfgsym {

extern const double pi;

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#if !HAVE_ASINH
SFGSYM_PUBLIC_API double asinh(const double d);
SFGSYM_PUBLIC_API Complex asinh(const Complex& c);
#endif
#if !HAVE_ACOSH
SFGSYM_PUBLIC_API double acosh(const double d);
SFGSYM_PUBLIC_API Complex acosh(const Complex& c);
#endif
#if !HAVE_ATANH
SFGSYM_PUBLIC_API double atanh(const double d);
SFGSYM_PUBLIC_API Complex atanh(const Complex& c);
#endif

SFGSYM_PUBLIC_API void complexMagnitude(double* magnitude, const kiss_fft_cpx* transformedSignal, unsigned len);
SFGSYM_PUBLIC_API void complexPhase(double* phase, const kiss_fft_cpx* transformedSignal, unsigned len);
SFGSYM_PUBLIC_API void copyDoublesIntoCpxBuffer(kiss_fft_cpx* target, const double* src, unsigned len);
SFGSYM_PUBLIC_API void multiplyCpxBuffer(kiss_fft_cpx* target, const double product, unsigned len);

template <class T, class U>
void copyBuffer(T* target, const U* src, unsigned len)
{
    while(len--)
        *target++ = *src++;
}

} // namespace sfgsym
