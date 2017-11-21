#pragma once

#include "tfp/config.hpp"
#include "tfp/math/RootPolynomial.hpp"
#include <iostream>

namespace tfp {

class CoefficientPolynomial;

class TransferFunction
{
public:
    struct PFEResultData
    {
        ComplexVector poles_;
        ComplexVector residuals_;
        RealVector directTerms_;
    };

    TransferFunction();
    TransferFunction(const CoefficientPolynomial& numerator, const CoefficientPolynomial& denominator);
    TransferFunction(const RootPolynomial& numerator, const RootPolynomial& denominator);

    void resize(int numRoots, int numPoles);
    int roots() const;
    int poles() const;

    void setRoot(int index, Complex value);
    void setPole(int index, Complex value);
    void setFactor(Real factor);
    Complex root(int index) const;
    Complex pole(int index) const;
    Real factor() const;

    Complex evaluate(Complex value) const;

    PFEResultData partialFractionExpansion(int numZeroPoles=0) const;
    void inverseLaplaceTransform(const PFEResultData& pfe, Real* outRealime, Real* outAmp, Real timeBegin, Real timeEnd, int numPoints) const;

    void impulseResponse(Real* outRealime, Real* outAmp, Real timeBegin, Real timeEnd, int numPoints) const;
    void stepResponse(Real* outRealime, Real* outAmp, Real timeBegin, Real timeEnd, int numPoints) const;
    void frequencyResponse(Real* outFreq, Real* outAmp, Real* outPhase, Real freqStart, Real freqEnd, int numPoints) const;

    friend std::ostream& operator<<(std::ostream& os, const TransferFunction& tf);

private:
    RootPolynomial numerator_;
    RootPolynomial denominator_;
};

} // namespace tfp
