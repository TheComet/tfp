#pragma once

#include "tfp/config.hpp"
#include "Eigen/Eigenvalues"
#include <vector>

namespace tfp {

class CoefficientPolynomial;
class RootPolynomial;

class UniqueRootIterator
{
public:
    const Complex& get() const;
    bool hasNext();
    void next();

private:
    friend class RootPolynomial;

    UniqueRootIterator(const RootPolynomial* rootPolynomial);

    const RootPolynomial* rootPolynomial_;
    int index_;
};

/*!
 * @brief A polynomial function consisting of roots (as opposed to
 * coefficients, see CoefficientPolynomial).
 */
class RootPolynomial
{
public:
    RootPolynomial();
    RootPolynomial(int size);
    RootPolynomial(const ComplexVector& roots, Real factor);
    RootPolynomial(const CoefficientPolynomial& coeffs);

    /*!
     * @brief Resizes the internal storage of roots. If expanding, the new
     * slots remain unintialised, so you must set them by calling root().
     */
    void resize(int size);

    //! Returns the number of roots (**including** multiple roots)
    int size() const;
    //! Sets the value of a root at the specified index
    void setRoot(int index, Complex value);
    //! Retrieves a root at the specified index.
    Complex root(int index) const;

    /*!
     * @brief Returns the multiplicity of the specified root. When setting the
     * value of a root, it is checked against the existing roots. If an
     * identical root is found, then its multiplicity is increased.
     */
    int multiplicity(int index);

    UniqueRootIterator uniqueRoots();

    void setFactor(Real factor);

    /*!
     * @brief  Returns the constant factor which was factored out when
     * calculating the monic polynomial. E.g.
     *
     * 2s + 4  -->  2(s+2)    factor=2
     */
    Real factor() const;

    /*!
     * Computes the polynomial coefficients with the specified roots.
     * This was ported from matlab's poly() function
     * Type ">> edit poly" and scroll to line 35.
     * @param roots Roots.
     * @return Polynomial coefficients.
     */
    CoefficientPolynomial poly() const;

    Complex evaluate(Complex value) const;

    friend std::ostream& operator<<(std::ostream& os, const RootPolynomial& polynomial);
    friend class UniqueRootIterator;

private:
    void updateMultiplicities();

    ComplexVector roots_;
    std::vector<Real> multiplicities_;
    Real factor_;
    bool multiplicityDirty_;
};

} // namespace tfp
