#include "tfp/models/TransferFunction.hxx"
#include <cmath>

namespace tfp {

// ----------------------------------------------------------------------------
template <class T>
TransferFunction<T>::TransferFunction()
{
}

// ----------------------------------------------------------------------------
template <class T>
TransferFunction<T>::TransferFunction(const CoefficientPolynomial<T>& numerator,
                                      const CoefficientPolynomial<T>& denominator) :
    numerator_(numerator.roots()),
    denominator_(denominator.roots())
{
}

// ----------------------------------------------------------------------------
template<class T>
TransferFunction<T>::TransferFunction(const PolynomialType& numerator,
                                      const PolynomialType& denominator) :
    numerator_(numerator),
    denominator_(denominator)
{
}

// ----------------------------------------------------------------------------
template <class T>
std::complex<T> TransferFunction<T>::evaluate(const std::complex<T>& value) const
{
    return numerator_.evaluate(value) / denominator_.evaluate(value);
}

// ----------------------------------------------------------------------------
template <class T>
typename TransferFunction<T>::PFEResultData
TransferFunction<T>::partialFractionExpansion(int numZeroPoles) const
{
    PFEResultData result;

    // Create poles vector. Add as many zero poles as specified
    result.poles_.resize(denominator_.roots_.size() + numZeroPoles, 1);
    for (int i = 0; i < denominator_.roots_.size(); ++i)
        result.poles_(i) = denominator_.roots_(i);
    for (int i = 0; i < numZeroPoles; ++i)
        result.poles_(denominator_.roots_.size() + i) = typename Type<T>::Complex(0, 0);

    // Calculate the direct terms, if necessary
    int numeratorDegree   = numerator_.roots_.size();
    int denominatorDegree = result.poles_.size();
    if(numeratorDegree >= denominatorDegree)
    {
        // TODO need to calculate the direct terms
    }

    result.residuals_.resize(denominatorDegree, 1);
    typename Type<T>::ComplexArray otherPoles(denominatorDegree - 1, 1);
    for (int m = 0; m < denominatorDegree; ++m)
    {
        for (int i = 0, j = 0; j < denominatorDegree; ++j)
        {
            if (j == m)
                continue;
            otherPoles(i) = result.poles_(j);
            ++i;
        }

        std::cout << "m: " << m << std::endl;
        std::cout << "poles:\n" << result.poles_ << std::endl;
        std::cout << "otherPoles:\n" << otherPoles << std::endl;

        RootPolynomial<T> aTilde(otherPoles, 1);
        std::complex<T> pvB = numerator_.evaluate(result.poles_(m));
        std::complex<T> pvA = aTilde.evaluate(result.poles_(m));
        std::cout << "pvB: " << pvB << std::endl;
        std::cout << "pvA: " << pvB << std::endl;
        std::complex<T> pvD = pvB / pvA;
        result.residuals_(m) = pvD / denominator_.factor_ * numerator_.factor_;
    }

    std::cout << "poles:\n" << result.poles_ << std::endl;
    std::cout << "residuals:\n" << result.residuals_ << std::endl;

    return result;
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::RealArray TransferFunction<T>::inverseLaplaceTransform(
        const PFEResultData& pfe,
        T timeBegin,
        T timeEnd,
        int numPoints
    ) const
{
    typename Type<T>::RealArray result(numPoints, 1);

    /*
     *                            a
     *  E(t)ae^(-at)    o--o    -----
     *                          s + a
     */
    for (int i = 0; i < numPoints; ++i)
    {
        T time = (timeEnd - timeBegin) * T(i) / (numPoints-1);
        typename Type<T>::Complex value = 0;
        for (int m = 0; m < pfe.residuals_.size(); ++m)
        {
            typename Type<T>::Complex a = -pfe.poles_(m);
            typename Type<T>::Complex k = pfe.residuals_(m);

            value += k * std::exp(-a * time);
        }
        result(i) = value.real();
    }

    return result;
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::RealArray TransferFunction<T>::impulseResponse(T timeBegin, T timeEnd, int numPoints) const
{
    PFEResultData pfe = partialFractionExpansion();
    return inverseLaplaceTransform(pfe, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::RealArray TransferFunction<T>::stepResponse(T timeBegin, T timeEnd, int numPoints) const
{
    // Add 1 zero-pole, which is the equivalent of integrating the function
    // once in the time domain (because this is the step response)
    PFEResultData pfe = partialFractionExpansion(1);
    return inverseLaplaceTransform(pfe, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::ComplexArray
TransferFunction<T>::frequencyResponse(T freqStart, T freqEnd, int numPoints) const
{
    typename Type<T>::ComplexArray result(numPoints, 1);

    for (int i = 0; i < numPoints; ++i)
    {
        T freq = freqStart * std::pow(freqEnd / freqStart, T(i) / (numPoints-1));
        result(i) = evaluate(std::complex<T>(0, freq));
    }

    return result;
}

// ----------------------------------------------------------------------------
template <class U>
std::ostream& operator<<(std::ostream& os, const TransferFunction<U>& tf)
{
    os << tf.numerator_ << '\n' << tf.denominator_;
    return os;
}

} // namespace tfp
