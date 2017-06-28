#pragma once

#include "tfp/math/CoefficientPolynomial.hxx"
#include "tfp/math/RootPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T>::CoefficientPolynomial()
{
}

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T>::CoefficientPolynomial(int size) :
    coefficients_(size, 1)
{
}

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T>::CoefficientPolynomial(const typename Type<T>::RealVector& coeffs) :
    coefficients_(coeffs)
{
}

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T>::CoefficientPolynomial(const RootPolynomial<T>& polynomial) :
    coefficients_(polynomial.poly().coefficients_)
{
}

// ----------------------------------------------------------------------------
template <class T>
void CoefficientPolynomial<T>::resize(int size)
{
    coefficients_.resize(size, Eigen::NoChange);
}

// ----------------------------------------------------------------------------
template <class T>
T& CoefficientPolynomial<T>::operator()(int index)
{
    return coefficients_(index, 0);
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T> CoefficientPolynomial<T>::roots() const
{
    // Companion matrix is a square m x m matrix, where m = polynomial order
    int size = coefficients_.rows() - 1;

    // Each coefficient is divided by this factor to make it a monic polynomial
    T factor = coefficients_(0);

    // Zero-order polynomial has no roots, but it still has a factor equal to
    // the largest coefficient
    if (size == 0)
        return RootPolynomial<T>(typename Type<T>::ComplexVector(), factor);

    if (companionMatrix_.rows() != coefficients_.rows())
        companionMatrix_ = CompanionMatrixType::Zero(size, size);

    // Has to be a monic polynomial (this is also why we pass by value instead of by const reference)
    typename Type<T>::RealVector monicCoeffs = coefficients_ / factor;

    for (int row = 1; row < size; ++row)
    {
        int column = row - 1;
        companionMatrix_(row, size-1) = -monicCoeffs(size - row);
        companionMatrix_(row, column) = 1;
    }
    companionMatrix_(0, size-1) = -monicCoeffs(size);

    EigensolverType solver;
    solver.compute(companionMatrix_, false);
    return RootPolynomial<T>(solver.eigenvalues(), factor);
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::Complex CoefficientPolynomial<T>::evaluate(const typename Type<T>::Complex& value) const
{
    return 0;
}

// ----------------------------------------------------------------------------
template <class U>
std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial<U>& polynomial)
{
    os << polynomial.coefficients_;
    return os;
}

} // namespace tfp
