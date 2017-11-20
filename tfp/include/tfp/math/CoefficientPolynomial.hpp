#pragma once

#include "tfp/config.hpp"
#include "tfp/math/RootPolynomial.hpp"

namespace tfp {


class CoefficientPolynomial
{
public:
    typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> CompanionMatrixType;
    typedef Eigen::EigenSolver<CompanionMatrixType> EigensolverType;

    CoefficientPolynomial();
    CoefficientPolynomial(int size);
    CoefficientPolynomial(const RealVector& coeffs);
    CoefficientPolynomial(const RootPolynomial& polynomial);

    void resize(int size);
    int size() const;
    void setCoefficient(int index, Real coefficient);
    Real coefficient(int index) const;

    RootPolynomial roots() const;

    Complex evaluate(const Complex& value) const;

    friend std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial& polynomial);

private:
    RealVector coefficients_;
    mutable CompanionMatrixType companionMatrix_;
};

} // namespace tfp
