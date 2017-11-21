#include "tfp/math/CoefficientPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
CoefficientPolynomial::CoefficientPolynomial() :
    coefficients_(Eigen::Array<Real, 1, 1>::Zero())
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
    /*
     * Companion matrix is a square m x m matrix, where m = polynomial order.
     * NOTE: The size should never be less than 1.
     */
    assert(coefficients_.rows() > 0);
    int size = coefficients_.rows() - 1;

    /*
     * Divide each coefficient by the highest order coefficient, making it a
     * monic polynomial. This is important for the companion matrix to work.
     */
    Real cn = coefficients_(size);
    RealVector monicCoeffs = coefficients_ / cn;

    /*
     * Zero-order polynomial has no roots, but it still has a factor equal to
     * the largest coefficient.
     */
    if (size == 0)
        return RootPolynomial(ComplexVector(), cn);

    // Build the companion matrix
    if (companionMatrix_.rows() != coefficients_.rows())
        companionMatrix_ = CompanionMatrixType::Zero(size, size);
    for (int row = 1; row < size; ++row)
    {
        int column = row - 1;
        companionMatrix_(row, size-1) = -monicCoeffs(size - row);
        companionMatrix_(row, column) = 1;
    }
    companionMatrix_(0, size-1) = -monicCoeffs(size);

    /*
     * Solve eigenvalues of companion matrix, which should be the roots of the
     * polynomial.
     */
    EigensolverType solver;
    solver.compute(companionMatrix_, false);
    return RootPolynomial(solver.eigenvalues(), cn);
}

// ----------------------------------------------------------------------------
Complex CoefficientPolynomial::evaluate(Complex value) const
{
    assert(0);
    return 0;
}

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial& polynomial)
{
    os << polynomial.coefficients_;
    return os;
}

} // namespace tfp
