#include "tfp/math/TransferFunction.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/RootPolynomial.hpp"
#include <cmath>

namespace tfp {

// ----------------------------------------------------------------------------
TransferFunction::TransferFunction()
{
}

// ----------------------------------------------------------------------------
TransferFunction::TransferFunction(const CoefficientPolynomial& numerator,
                                   const CoefficientPolynomial& denominator) :
    numerator_(numerator.roots()),
    denominator_(denominator.roots())
{
}

// ----------------------------------------------------------------------------
TransferFunction::TransferFunction(const RootPolynomial& numerator,
                                   const RootPolynomial& denominator) :
    numerator_(numerator),
    denominator_(denominator)
{
}

// ----------------------------------------------------------------------------
void TransferFunction::resize(int numRoots, int numPoles)
{
    numerator_.resize(numRoots);
    denominator_.resize(numPoles);
}

// ----------------------------------------------------------------------------
int TransferFunction::roots() const
{
    return numerator_.size();
}

// ----------------------------------------------------------------------------
int TransferFunction::poles() const
{
    return denominator_.size();
}

// ----------------------------------------------------------------------------
void TransferFunction::setRoot(int index, Complex value)
{
    numerator_.setRoot(index, value);
}

// ----------------------------------------------------------------------------
void TransferFunction::setPole(int index, Complex value)
{
    denominator_.setRoot(index, value);
}

// ----------------------------------------------------------------------------
void TransferFunction::setFactor(Real factor)
{
    numerator_.setFactor(factor);
    denominator_.setFactor(1.0);
}

// ----------------------------------------------------------------------------
Complex TransferFunction::root(int index) const
{
    return numerator_.root(index);
}

// ----------------------------------------------------------------------------
Complex TransferFunction::pole(int index) const
{
    return denominator_.root(index);
}

// ----------------------------------------------------------------------------
Real TransferFunction::factor() const
{
    return numerator_.factor() / denominator_.factor();
}

// ----------------------------------------------------------------------------
Complex TransferFunction::evaluate(Complex value) const
{
    return numerator_.evaluate(value) / denominator_.evaluate(value);
}

// ----------------------------------------------------------------------------
TransferFunction::PFEResultData
TransferFunction::partialFractionExpansion(int numZeroPoles) const
{
    PFEResultData result;

    // Create poles vector. Add as many zero poles as specified
    result.poles_.resize(denominator_.size() + numZeroPoles, 1);
    for (int i = 0; i < denominator_.size(); ++i)
        result.poles_(i) = denominator_.root(i);
    for (int i = 0; i < numZeroPoles; ++i)
        result.poles_(denominator_.size() + i) = Complex(0, 0);

    // Calculate the direct terms, if necessary
    int numeratorDegree   = numerator_.size();
    int denominatorDegree = result.poles_.size();
    if(numeratorDegree >= denominatorDegree)
    {
        // TODO need to calculate the direct terms
    }

    // Calculate residuals
    result.residuals_.resize(denominatorDegree, 1);
    ComplexArray otherPoles(denominatorDegree - 1, 1);
    for (int m = 0; m < denominatorDegree; ++m)
    {
        for (int i = 0, j = 0; j < denominatorDegree; ++j)
        {
            if (j == m)
                continue;
            otherPoles(i) = result.poles_(j);
            ++i;
        }

        RootPolynomial aTilde(otherPoles, 1);
        Complex pvB = numerator_.evaluate(result.poles_(m));
        Complex pvA = aTilde.evaluate(result.poles_(m));
        Complex pvD = pvB / pvA;
        result.residuals_(m) = pvD / denominator_.factor();
    }

    return result;
}

// ----------------------------------------------------------------------------
void TransferFunction::inverseLaplaceTransform(
        const PFEResultData& pfe,
        Real* outTime,
        Real* outAmp,
        Real timeBegin,
        Real timeEnd,
        int numPoints
    ) const
{
    /*
     *                            a
     *  E(t)ae^(-at)    o--o    -----
     *                          s + a
     */
    for (int i = 0; i < numPoints; ++i)
    {
        outTime[i] = (timeEnd - timeBegin) * Real(i) / (numPoints-1);
        outAmp[i] = 0;
        for (int m = 0; m < pfe.residuals_.size(); ++m)
        {
            Complex a = pfe.poles_(m);
            Complex k = pfe.residuals_(m);

            // We're only interested in calculating the real part, since the
            // imaginary parts will cancel out to 0 if the function is real.
            //     Equivalent to:   outAmp[i] += k * std::exp(a * outTime[i]);
            outAmp[i] += std::exp(a.real()*outTime[i]) * (k.real() * std::cos(a.imag()*outTime[i]) - k.imag()*std::sin(a.imag()*outTime[i]));
            //outAmp[i] += (k * std::exp(a * outTime[i])).real();
        }
    }
}

// ----------------------------------------------------------------------------
void TransferFunction::impulseResponse(Real* outRealime, Real* outAmp, Real timeBegin, Real timeEnd, int numPoints) const
{
    PFEResultData pfe = partialFractionExpansion();
    return inverseLaplaceTransform(pfe, outRealime, outAmp, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
void TransferFunction::stepResponse(Real* outRealime, Real* outAmp, Real timeBegin, Real timeEnd, int numPoints) const
{
    // Add 1 zero-pole, which is the equivalent of integrating the function
    // in the time domain (because this is the step response)
    PFEResultData pfe = partialFractionExpansion(1);
    return inverseLaplaceTransform(pfe, outRealime, outAmp, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
void TransferFunction::frequencyResponse(Real* outFreq, Real* outAmp, Real* outPhase, Real freqStart, Real freqEnd, int numPoints) const
{
    for (int i = 0; i < numPoints; ++i)
    {
        outFreq[i] = freqStart * std::pow(freqEnd / freqStart, Real(i) / (numPoints-1));
        Complex result = evaluate(Complex(0, outFreq[i]));
        outAmp[i] = 20 * std::log10(std::abs(result));
        outPhase[i] = std::arg(result) * 180.0 / M_PI;
    }
}

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const TransferFunction& tf)
{
    os << tf.numerator_ << '\n' << tf.denominator_;
    return os;
}

} // namespace tfp
