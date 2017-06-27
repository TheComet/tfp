#pragma once

#include "tfp/config.hpp"
#include "tfp/models/Type.hpp"
#include "Eigen/Dense"
#include <iostream>

namespace tfp {

template <class T> class RootPolynomial;

template <class T>
class CoefficientPolynomial
{
public:
    typedef typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> CompanionMatrixType;
    typedef typename Eigen::EigenSolver<CompanionMatrixType> EigensolverType;

    CoefficientPolynomial();
    CoefficientPolynomial(int size);
    CoefficientPolynomial(const typename Type<T>::RealVector& coeffs);
    CoefficientPolynomial(const RootPolynomial<T>& polynomial);

    void resize(int size);
    T& operator()(int index);

    RootPolynomial<T> roots() const;

    typename Type<T>::Complex evaluate(const typename Type<T>::Complex& value) const;

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial<U>& polynomial);

    typename Type<T>::RealVector coefficients_;

private:
    mutable CompanionMatrixType companionMatrix_;
};

} // namespace tfp
