#pragma once

#include "tfp/math/RootPolynomial.hxx"
#include "tfp/math/CoefficientPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial() :
    factor_(1)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(int size) :
    roots_(size, 1),
    factor_(1)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(const typename Type<T>::ComplexVector& roots, T factor) :
    roots_(roots),
    factor_(factor)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(const CoefficientPolynomial<T>& polynomial) :
    roots_(polynomial.roots().roots_),
    factor_(polynomial.coefficients_(0))
{
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::resize(int size)
{
    roots_.resize(size, Eigen::NoChange);
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::Complex& RootPolynomial<T>::operator()(int index)
{
    return roots_(index, 0);
}

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T> RootPolynomial<T>::poly() const
{
    // Need complex buffers for the computation, even though the resulting
    // coefficients are real (since we're only dealing with complex-conjugate)
    // roots (real functions)
    typename Type<T>::ComplexVector coeffs = Type<T>::ComplexVector::Zero(roots_.size() + 1);
    typename Type<T>::ComplexVector temp(roots_.size() + 1);
    coeffs(0) = Type<T>::Complex(1, 0);

    for (int i = 0; i < roots_.size(); ++i)
    {
        // multiply coefficients with current root and store in temp buffer
        for (int j = 0; j < coeffs.size(); ++j) {
            temp(j) = roots_(i) * coeffs(j);
        }
        // subtract temp buffer from coefficients
        for (int j = 1; j < coeffs.size(); ++j) {
            coeffs(j) -= temp(j - 1);
        }
    }

    // There should be no more complex values.
    CoefficientPolynomial<T> ret;
    ret.resize(coeffs.size());
    for (int i = 0; i < coeffs.size(); ++i)
        ret.coefficients_(i) = coeffs(i).real() * factor_;

    return ret;
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::Complex RootPolynomial<T>::evaluate(const typename Type<T>::Complex& value) const
{
    if (roots_.size() == 0)
        return typename Type<T>::Complex(factor_, 0);

    typename Type<T>::ComplexArray factors(roots_.size(), 1);
    factors = value - typename Type<T>::ComplexArray(roots_);
    typename Type<T>::Complex ret = factors(0);
    for (int i = 1; i < factors.size(); ++i)
        ret *= factors(i);
    return ret * factor_;
}

// ----------------------------------------------------------------------------
template <class U>
std::ostream& operator<<(std::ostream& os, const RootPolynomial<U>& polynomial)
{
    os << polynomial.roots_;
    return os;
}

} // namespace tfp
