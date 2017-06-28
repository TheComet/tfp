#include "tfp/math/Math.hpp"
#include <cmath>

// ----------------------------------------------------------------------------
namespace std {
    double atof(const std::string& str) {
        return atof(str.c_str());
    }
}


namespace tfp {

const double pi = 2 * std::acos(0.0);

// ----------------------------------------------------------------------------
void complexMagnitude(double* magnitude, const kiss_fft_cpx* transformedSignal, unsigned len)
{
    while(len--)
    {
        magnitude[len] = std::sqrt(
            transformedSignal[len].r * transformedSignal[len].r +
            transformedSignal[len].i * transformedSignal[len].i
        );
    }
}

// ----------------------------------------------------------------------------
void complexPhase(double* phase, const kiss_fft_cpx* transformedSignal, unsigned len)
{
    while(len--)
        phase[len] = std::atan2(transformedSignal[len].i, transformedSignal[len].r);
}

// ----------------------------------------------------------------------------
void copyDoublesIntoCpxBuffer(kiss_fft_cpx* target, const double* src, unsigned len)
{
    while(len--)
    {
        target->i = 0;
        (target++)->r = *src++;
    }
}

// ----------------------------------------------------------------------------
void multiplyCpxBuffer(kiss_fft_cpx* target, const double product, unsigned len)
{
    while(len--)
    {
        target->i *= product;
        target->r *= product;
        ++target;
    }
}

} // namespace tfp
