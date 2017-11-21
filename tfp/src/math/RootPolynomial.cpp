#include "tfp/math/RootPolynomial.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
UniqueRootIterator::UniqueRootIterator(const RootPolynomial* rootPolynomial) :
    rootPolynomial_(rootPolynomial),
    index_(0)
{
}

// ----------------------------------------------------------------------------
const Complex& UniqueRootIterator::get() const
{
    return rootPolynomial_->roots_[index_];
}

// ----------------------------------------------------------------------------
bool UniqueRootIterator::hasNext()
{
    return index_ < rootPolynomial_->roots_.size();
}

// ----------------------------------------------------------------------------
void UniqueRootIterator::next()
{
    while (true)
    {
        ++index_;
        if (hasNext() == false)
            break;

        /* TODO
        if (rootPolynomial_->roots_[index_].first >= 1)
            break;*/
    }
}

// ----------------------------------------------------------------------------
RootPolynomial::RootPolynomial() :
    factor_(1),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
RootPolynomial::RootPolynomial(int size) :
    roots_(size, 1),
    factor_(1),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
RootPolynomial::RootPolynomial(const ComplexVector& roots, Real factor) :
    roots_(roots),
    factor_(factor),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
RootPolynomial::RootPolynomial(const CoefficientPolynomial& polynomial) :
    roots_(polynomial.roots().roots_),
    factor_(polynomial.coefficient(0)),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
void RootPolynomial::resize(int size)
{
    roots_.resize(size, Eigen::NoChange);
}

// ----------------------------------------------------------------------------
int RootPolynomial::size() const
{
    return roots_.size();
}

// ----------------------------------------------------------------------------
void RootPolynomial::setRoot(int index, Complex root)
{
    multiplicityDirty_ = true;
    roots_[index] = root;
}

// ----------------------------------------------------------------------------
Complex RootPolynomial::root(int index) const
{
    return roots_[index];
}

// ----------------------------------------------------------------------------
int RootPolynomial::multiplicity(int index)
{
    updateMultiplicities();

    int multiplicity = multiplicities_[index];
    if (multiplicity < 1)
        multiplicity = multiplicities_[-multiplicity];
    return multiplicity;
}

// ----------------------------------------------------------------------------
void RootPolynomial::updateMultiplicities()
{
    if (multiplicityDirty_)
    {
        multiplicities_.resize(roots_.size());
        for (int i = 0; i != multiplicities_.size(); ++i)
            multiplicities_[i] = 1;

        for (int i = 0; i != roots_.size(); ++i)
        {
            for (int j = i + 1; j < roots_.size(); ++j)
            {
                if (roots_[i] == roots_[j])
                {
                    if (multiplicities_[i] >= 1)
                    {
                        multiplicities_[i]++;
                        multiplicities_[j] = -i;
                    }
                }
            }
        }

        multiplicityDirty_ = false;
    }
}

// ----------------------------------------------------------------------------
UniqueRootIterator RootPolynomial::uniqueRoots()
{
    updateMultiplicities();
    return UniqueRootIterator(this);
}

// ----------------------------------------------------------------------------
void RootPolynomial::setFactor(Real factor)
{
    factor_ = factor;
}

// ----------------------------------------------------------------------------
Real RootPolynomial::factor() const
{
    return factor_;
}

// ----------------------------------------------------------------------------
CoefficientPolynomial RootPolynomial::poly() const
{
    // Need complex buffers for the computation, even though the resulting
    // coefficients are real (since we're only dealing with complex-conjugate)
    // roots (real functions)
    ComplexVector coeffs = ComplexVector::Zero(roots_.size() + 1);
    ComplexVector temp(roots_.size() + 1);
    coeffs(0) = Complex(1, 0);

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
    CoefficientPolynomial ret;
    ret.resize(coeffs.size());
    for (int i = 0; i < coeffs.size(); ++i)
        ret.setCoefficient(i, coeffs(i).real() * factor_);

    return ret;
}

// ----------------------------------------------------------------------------
Complex RootPolynomial::evaluate(Complex value) const
{
    if (roots_.size() == 0)
        return Complex(factor_, 0);

    ComplexArray factors(roots_.size(), 1);
    factors = value - ComplexArray(roots_);
    Complex ret = factors(0);
    for (int i = 1; i < factors.size(); ++i)
        ret *= factors(i);
    return ret * factor_;
}

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const RootPolynomial& polynomial)
{
    os << polynomial.roots_;
    return os;
}

} // namespace tfp
