#include "tfp/math/CoefficientPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
CoefficientPolynomial::CoefficientPolynomial()
{
}

// ----------------------------------------------------------------------------
CoefficientPolynomial::CoefficientPolynomial(int size) :
    coefficients_(size, 1)
{
}

// ----------------------------------------------------------------------------
CoefficientPolynomial::CoefficientPolynomial(const RealVector& coeffs) :
    coefficients_(coeffs)
{
}

// ----------------------------------------------------------------------------
CoefficientPolynomial::CoefficientPolynomial(const RootPolynomial& polynomial) :
    coefficients_(polynomial.poly().coefficients_)
{
}

// ----------------------------------------------------------------------------
void CoefficientPolynomial::resize(int size)
{
    coefficients_.resize(size, Eigen::NoChange);
}

// ----------------------------------------------------------------------------
int CoefficientPolynomial::size() const
{
    return coefficients_.size();
}

// ----------------------------------------------------------------------------
void CoefficientPolynomial::setCoefficient(int index, Real coefficient)
{
    coefficients_(index, 0) = coefficient;
}

// ----------------------------------------------------------------------------
Real CoefficientPolynomial::coefficient(int index) const
{
    return coefficients_(index, 0);
}

// ----------------------------------------------------------------------------
RootPolynomial CoefficientPolynomial::roots() const
{
    // Companion matrix is a square m x m matrix, where m = polynomial order
    int size = coefficients_.rows() - 1;
    if (size < 0)
        return RootPolynomial();

    // Each coefficient is divided by this factor to make it a monic polynomial
    Real factor = coefficients_(size);

    // Zero-order polynomial has no roots, but it still has a factor equal to
    // the largest coefficient
    if (size == 0)
        return RootPolynomial(ComplexVector(), factor);

    if (companionMatrix_.rows() != coefficients_.rows())
        companionMatrix_ = CompanionMatrixType::Zero(size, size);

    // Has to be a monic polynomial (this is also why we pass by value instead of by const reference)
    RealVector monicCoeffs = coefficients_ / factor;

    for (int row = 1; row < size; ++row)
    {
        int column = row - 1;
        companionMatrix_(row, size-1) = -monicCoeffs(size - row);
        companionMatrix_(row, column) = 1;
    }
    companionMatrix_(0, size-1) = -monicCoeffs(size);

    EigensolverType solver;
    solver.compute(companionMatrix_, false);
    return RootPolynomial(solver.eigenvalues(), factor);
}

// ----------------------------------------------------------------------------
Complex CoefficientPolynomial::evaluate(const Complex& value) const
{
    return 0;
}

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial& polynomial)
{
    os << polynomial.coefficients_;
    return os;
}

} // namespace tfp
