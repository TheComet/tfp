#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <cstring>

using namespace tfp;

// ----------------------------------------------------------------------------
void Expression::enforceProductLHS(const char* variable)
{
    if (left())  left()->enforceProductLHS(variable);
    if (right()) right()->enforceProductLHS(variable);
    if (isOperation(op::mul))
    {
        if (right() && right()->hasVariable(variable))
        {
            tfp::Reference<Expression> tmp = right_;
            right_ = left_;
            left_ = tmp;
        }
    }
}

// ----------------------------------------------------------------------------
bool Expression::enforceConstantExponent(const char* variable)
{
    /*
     * Performs the following transformations:
     *    s    --> s^1
     * If s is raised to a non-constant (e.g. s^a or (s+2)^a) abort and return false.
     */
    bool success = true;
    if (left())  success &= left()->enforceConstantExponent(variable);
    if (right()) success &= right()->enforceConstantExponent(variable);
    if (hasVariable(variable) == false)
        return success;

    // Travel up the tree in search of op::pow...
    for (Expression* e = parent(); e != NULL; e = e->parent())
        if (e->isOperation(op::pow))
            if (e->right()->type() != CONSTANT)  // RHS has to be constant
                return false;
    
    // If immediate parent is already pow (and has a constant RHS), we're done.
    if (parent()->isOperation(op::pow) == true)
        return true;
    
    set (op::pow, shallowClone(), Expression::make(1.0));
    return success;
}

// ----------------------------------------------------------------------------
bool Expression::expandConstantExponentsIntoProducts(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->expandConstantExponentsIntoProducts(variable);
    if (right()) weMatter |= right()->expandConstantExponentsIntoProducts(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Performs the following transformations:
     *   s^c  (c=const)  --> s*s*s*...
     *   s^-c (c=const)  --> (s*s*s*...)^-1
     */
    if (isOperation(op::pow) == false)
        return true;

    if (right()->type() != CONSTANT || std::floor(right()->value()) != right()->value())
        throw std::runtime_error("Expression is raised to a non-integer value, can't expand!");

    int exponent = (int)right()->value();

    if (exponent == 0)
    {
        set(1.0);
        return true;
    }

    if (exponent == 1 || exponent == -1)
    {
        return true;
    }

    Expression* toFactor = left();
    Expression* e = Expression::make(op::mul, toFactor->clone(), toFactor);
    int expandTimes = exponent < 0 ? -exponent : exponent;
    for (int i = 2; i != expandTimes; ++i)
    {
        e = Expression::make(op::mul, e, toFactor->clone());
    }

    if (exponent < 0)
    {
        e = Expression::make(op::pow, e, Expression::make(-1.0));
    }

    set(e);

    return true;
}

// ----------------------------------------------------------------------------
bool Expression::factorNegativeExponents(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->eliminateDivisionsAndSubtractions(variable);
    if (right()) weMatter |= right()->eliminateDivisionsAndSubtractions(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Does the following manipulations:
     *    x + ys^-c (c=const)   ->   s^-c(xs^c + y)
     *    x + ys^-c1 + zs^-c2 (c1=const,c2=const)  ->  s^-c(xs^c + ys^(c-c1) + zs^(c-c2)) (such that c-c1 >= 0 and c-c2 >= 0)
     */
    if (isOperation(op::pow) == false)
        return true;

}

// ----------------------------------------------------------------------------
bool Expression::eliminateDivisionsAndSubtractions(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->eliminateDivisionsAndSubtractions(variable);
    if (right()) weMatter |= right()->eliminateDivisionsAndSubtractions(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Performs the following transformations:
     *   a/s    -->  as^-1
     *   a/s^x  -->  as^-x
     *   a-s    -->  a+s*(-1)
     *   a-sx   -->  a+s*(-x)
     * This makes the tree easier to work with for later stages, because
     * the assumption that no division or subtraction operators exist can be
     * made.
     */
    static struct { op::Op2 outer; op::Op2 outerInv; op::Op2 inner; } ops[2] = {
        { op::div, op::mul, op::pow },
        { op::sub, op::add, op::mul }
    };

    for (int i = 0; i != 2; ++i)
    {
        if (isOperation(ops[i].outer) == false)
            continue;
        
        if (hasRHSOperation(ops[i].inner))
        {
            Expression* toNegate = right()->right();
            set(ops[i].outerInv, left(), right());
            if (toNegate->type() == CONSTANT)
                toNegate->set(-toNegate->value());
            else
                toNegate->set(op::mul, toNegate->shallowClone(), Expression::make(-1.0));

        }
        else
        {
            set(ops[i].outerInv,
                left(),
                Expression::make(ops[i].inner,
                    right(),
                    Expression::make(-1)));
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
bool Expression::eliminateNegativeExponents(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->eliminateNegativeExponents(variable);
    if (right()) weMatter |= right()->eliminateNegativeExponents(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Does the following manipulations:
     */

    if (parent() == NULL)
        return true;

    if (isOperation(op::pow) == false)
        return true;

    return true;
}

// ----------------------------------------------------------------------------
bool Expression::expand(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->expand(variable);
    if (right()) weMatter |= right()->expand(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Performs manipulations such as
     *   (a+s)(b+s)  -->  ab + as + sb + ss
     *   a(s+4)      -->  as + 4a
     *
     * Does not handle expressions with constant powers.
     */

    if (parent() == NULL)
        return true;
    
    if (isOperation(op::add) == false)
        return true;

    if (parent()->isOperation(op::mul))
    {
        Expression* factorInOther = getOtherOperand();
        Expression* factorInThis = factorInOther->clone();
        parent()->set(op::add, this, Expression::make(op::mul, factorInOther, right()));
        set(op::mul, factorInThis, left());

        factorInThis->expand(variable);
        factorInOther->expand(variable);
    }

    if (parent()->isOperation(op::pow))
    {
        parent()->expandConstantExponentsIntoProducts(variable);
    }

    return true;
}

// ----------------------------------------------------------------------------
bool Expression::manipulateIntoRationalFunction(const char* variable)
{
    /*
     * Create the "split" operator, i.e. this is the expression that splits the
     * numerator from the denominator. By default, use the last division
     * operator we can find in the tree. If none exists, create a div operator
     * that has no effect.
     */
    root()->dump("wtf.dot");
    Expression* split = find(op::div);
    if (split == NULL)
        split = Expression::make(op::div, split, Expression::make(1.0));
    root()->dump("wtf.dot", true);

    /*
     * Eliminate divisions and subtractions on both sides of the split, so we
     * can start shuffling terms back and forth between the numerator and
     * denominator without having to deal with lots of edge cases.
     */
    split->left()->eliminateDivisionsAndSubtractions(variable);
    split->right()->eliminateDivisionsAndSubtractions(variable);
    root()->dump("wtf.dot", true);

    /*
     * All op::pow operations with our variable on the LHS need to have a
     * constant RHS. If not, error out, because such an expression cannot be
     * reduced to a rational function.
     */
    /*if (enforceConstantExponent(variable) == false)
        return false;*/
        //throw std::runtime_error("This expression has variable exponents! These cannot be reduced to a rational function.");
    expandConstantExponentsIntoProducts(variable);
    root()->dump("wtf.dot", true);

    split->left()->expand(variable);
    split->right()->expand(variable);
    root()->dump("wtf.dot", true);

    return false;
}

// ----------------------------------------------------------------------------
Expression::TransferFunctionCoefficients Expression::calculateTransferFunctionCoefficients(const char* variable)
{
    manipulateIntoRationalFunction(variable);
    
    return TransferFunctionCoefficients();
}

// ----------------------------------------------------------------------------
tfp::TransferFunction<double>
Expression::calculateTransferFunction(const TransferFunctionCoefficients& tfe,
                                      const VariableTable* vt)
{
    tfp::CoefficientPolynomial<double> numerator(tfe.numeratorCoefficients_.size());
    tfp::CoefficientPolynomial<double> denominator(tfe.denominatorCoefficients_.size());

    for (std::size_t i = 0; i != tfe.numeratorCoefficients_.size(); ++i)
    {
        double value = tfe.numeratorCoefficients_[i]->evaluate(vt);
        numerator.setCoefficient(i, value);
    }

    for (std::size_t i = 0; i != tfe.denominatorCoefficients_.size(); ++i)
    {
        double value = tfe.denominatorCoefficients_[i]->evaluate(vt);
        numerator.setCoefficient(i, value);
    }

    return tfp::TransferFunction<double>(numerator, denominator);
}
