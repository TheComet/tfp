#pragma once

#include "tfp/config.hpp"
#include "tfp/models/RootPolynomial.hpp"
#include "tfp/models/CoefficientPolynomial.hpp"
#include <iostream>

namespace tfp {

template <class T>
class TransferFunction
{
public:
    typedef RootPolynomial<T> PolynomialType;

    struct PFEResultData
    {
        typename Type<T>::ComplexVector poles_;
        typename Type<T>::ComplexVector residuals_;
        typename Type<T>::RealVector directTerms_;
    };

    TransferFunction();
    TransferFunction(const CoefficientPolynomial<T>& numerator, const CoefficientPolynomial<T>& denominator);
    TransferFunction(const RootPolynomial<T>& numerator, const RootPolynomial<T>& denominator);

    std::complex<T> evaluate(const std::complex<T>& value) const;

    PFEResultData partialFractionExpansion(int numZeroPoles=0) const;
    typename Type<T>::RealArray inverseLaplaceTransform(
        const PFEResultData& pfe,
        T timeBegin,
        T timeEnd,
        int numPoints) const;

    typename Type<T>::RealArray impulseResponse(T timeBegin, T timeEnd, int numPoints) const;
    typename Type<T>::RealArray stepResponse(T timeBegin, T timeEnd, int numPoints) const;
    typename Type<T>::ComplexArray frequencyResponse(T freqStart, T freqEnd, int numPoints) const;

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const TransferFunction<U>& tf);

    RootPolynomial<T> numerator_;
    RootPolynomial<T> denominator_;
};

} // namespace tfp
