#pragma once

#include "tfp/config.hpp"
#include "tfp/math/RootPolynomial.hpp"

namespace tfp {

/*!
 * @brief Represents a polynomial of the form c0 + c1*s + c2*s^2 + c3*s^3...
 * The coefficients are stored in ascending order (p.coefficient(0) is c0).
 */
class CoefficientPolynomial
{
public:
    typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> CompanionMatrixType;
    typedef Eigen::EigenSolver<CompanionMatrixType> EigensolverType;

    /*!
     * @brief The polynomial is constructed with a single coefficient that is
     * set to 0.
     */
    CoefficientPolynomial();

    /*!
     * @brief The polynomial is constructed with the specified size, but not
     * initialised.
     */
    CoefficientPolynomial(int size);

    /*!
     * @brief Construct from an existing vector of coefficients. The first
     * entry in the vector corresponds to coefficient c0.
     */
    CoefficientPolynomial(const RealVector& coeffs);

    /*!
     * @brief Construct from a polynomial with root representation.
     */
    CoefficientPolynomial(const RootPolynomial& polynomial);

    /*!
     * @brief Updates the number of coefficients this polynomial has. If the
     * size is larger than before, then the new coefficients are uninitialised.
     */
    void resize(int size);

    /*!
     * @return Get the number of coefficients this polynomial has.
     */
    int size() const;

    /*!
     * @brief Sets the value of a specified coefficient.
     * @param[in] index Index of coefficient, starting at 0 for c0.
     * @param[in] coefficient The value of the coefficient to set.
     */
    void setCoefficient(int index, Real coefficient);

    /*!
     * @brief Retrieve the value of a coefficient at the specified index.
     */
    Real coefficient(int index) const;

    /*!
     * @brief Computes the roots of the polynomial and returns a new polynomial
     * object.
     *
     * The roots are calculated with Eigen3 by building a companion matrix and
     * solving for the eigenvalues.
     */
    RootPolynomial roots() const;

    Complex evaluate(Complex value) const;

    friend std::ostream& operator<<(std::ostream& os, const CoefficientPolynomial& polynomial);

private:
    RealVector coefficients_;
    mutable CompanionMatrixType companionMatrix_;
};

} // namespace tfp
