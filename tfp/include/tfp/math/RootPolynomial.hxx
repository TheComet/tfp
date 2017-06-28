#pragma once

#include "tfp/config.hpp"
#include "tfp/math/Type.hpp"
#include "Eigen/Eigenvalues"
#include <iostream>

namespace tfp {

template <class T> class CoefficientPolynomial;

template <class T>
class RootPolynomial
{
public:
    RootPolynomial();
    RootPolynomial(int size);
    RootPolynomial(const typename Type<T>::ComplexVector& roots, T factor);
    RootPolynomial(const CoefficientPolynomial<T>& coeffs);

    void resize(int size);
    typename Type<T>::Complex& operator()(int index);

    /*!
     * Computes the polynomial coefficients with the specified roots.
     * This was ported from matlab's poly() function
     * Type ">> edit poly" and scroll to line 35.
     * @param roots Roots.
     * @return Polynomial coefficients.
     */
    CoefficientPolynomial<T> poly() const;

    typename Type<T>::Complex evaluate(const typename Type<T>::Complex& value) const;

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const RootPolynomial<U>& polynomial);

    typename Type<T>::ComplexVector roots_;
    T factor_;
};

} // namespace tfp
