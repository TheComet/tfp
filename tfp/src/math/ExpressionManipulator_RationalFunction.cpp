#include "tfp/math/ExpressionManipulators.hpp"
#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <cstring>

using namespace tfp;

// ----------------------------------------------------------------------------
bool TFManipulator::factorNegativeExponentsToNumerator(Expression* e, Expression* numerator, const char* variable)
{
    if (e->isOperation(op::mul))
    {
        return factorNegativeExponentsToNumerator(e->left(), numerator, variable) |
               factorNegativeExponentsToNumerator(e->right(), numerator, variable);
    }

    if (e->isOperation(op::pow) == false)
        return false;

    if (e->right()->type() != Expression::CONSTANT || e->right()->value() >= 0.0)
        return false;

    e->right()->set(-e->right()->value());
    factorIn(numerator, e);
    e->getOtherOperand()->collapseIntoParent();
    return true;
}

// ----------------------------------------------------------------------------
inline bool logically_equal(double a, double b, double error_factor=1.0)
{
  return a==b ||
    std::abs(a-b)<std::abs(std::min(a,b))*std::numeric_limits<double>::epsilon()*
                  error_factor;
}
bool TFManipulator::manipulateIntoRationalFunction(Expression* e, const char* variable)
{
    /*
     * Create the "split" operator, i.e. this is the expression that splits the
     * numerator from the denominator. By default, use the last division
     * operator we can find in the tree. If none exists, create a div operator
     * that has no effect.
     */
    Expression* split = e->type() == Expression::FUNCTION2 && e->op2() == op::div ?
            e : Expression::make(op::div, e, Expression::make(1.0));

    /*
     * Eliminate divisions and subtractions on both sides of the split, so we
     * can start shuffling terms back and forth between the numerator and
     * denominator without having to deal with lots of edge cases.
     */
    recursivelyCall(&TFManipulator::eliminateDivisionsAndSubtractions, split->left(), variable);
    recursivelyCall(&TFManipulator::eliminateDivisionsAndSubtractions, split->right(), variable);

    /*
     * All op::pow operations with our variable on the LHS need to have a
     * constant RHS. If not, error out, because such an expression cannot be
     * reduced to a rational function.
     */
    /*if (enforceConstantExponent(variable) == false)
        return false;*/
        //throw std::runtime_error("This expression has variable exponents! These cannot be reduced to a rational function.");
    ExpressionOptimiser optimise;
    while (true)
    {
        while (recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split->right(), variable)) {}
        while (recursivelyCall(&TFManipulator::expand, split->right(), variable)) {}
        optimise.constants(split->right());
        optimise.uselessOperations(split->right());

        while (recursivelyCall(&TFManipulator::factorNegativeExponents, split->right(), variable)) {}

        if (factorNegativeExponentsToNumerator(split->right(), split->left(), variable) == false)
            break;
    }

    while (optimise.uselessOperations(split->left()) |
           recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split, variable) |
           recursivelyCall(&TFManipulator::expand, split, variable))
    {}

    optimise.everything(e);

    return false;
}

// ----------------------------------------------------------------------------
typedef std::vector< Reference<Expression> > ExpressionList;
static void splitSums(ExpressionList* sums, Expression* e)
{
    if (e->isOperation(op::add))
    {
        splitSums(sums, e->left());
        splitSums(sums, e->right());
    }
    else
    {
        sums->push_back(e->unlinkFromTree());
    }
}

static void resizeCoefficientList(ExpressionList* coeffs, std::size_t size)
{
    while (coeffs->size() < size)
        coeffs->push_back(NULL);
}

static void combineSumsIntoCoefficients(ExpressionList* coeffs, const ExpressionList& sums, const char* variable)
{
    for (ExpressionList::const_iterator it = sums.begin(); it != sums.end(); ++it)
    {
        int order = 0;
        
        // Determine coefficient order by finding the variable we're a function of and checking its exponent.
        Expression* var = (*it)->find(variable);
        if (var != NULL)
        {
            if (var->parent() && var->parent()->isOperation(op::pow))
            {
                if (var->parent()->right()->type() != Expression::CONSTANT)
                    throw ExpressionManipulator::NonConstantExponentException("Can't calculate order.");
                order = (int)var->parent()->right()->value();
                
                // Eliminate variable and power expression so we are left with the coefficient only
                if (var->parent()->parent())
                    var->parent()->getOtherOperand()->collapseIntoParent();
            }
            else
            {
                // No exponent is the same as an exponent of 1
                order = 1;
                // Eliminate variable so we are left with the coefficient only
                if (var->parent())
                    var->parent()->getOtherOperand()->collapseIntoParent();
            }
        }

        resizeCoefficientList(coeffs, order + 1);
        (*coeffs)[order] = (*coeffs)[order] == NULL ? *it : Expression::make(op::add, (*coeffs)[0], *it);
    }
}

TFManipulator::TFCoefficients
TFManipulator::calculateTransferFunctionCoefficients(Expression* e, const char* variable)
{
    manipulateIntoRationalFunction(e, variable);
    e->dump("wtf.dot", true);

    ExpressionList numSums;
    ExpressionList denSums;
    splitSums(&numSums, e->left());
    splitSums(&denSums, e->right());
    
    for (std::size_t i = 0; i != numSums.size(); ++i)
    {
        std::stringstream ss; ss << "(sums) numerator degree: " << i;
        numSums[i]->dump("wtf.dot", true, ss.str().c_str());
    }
    for (std::size_t i = 0; i != denSums.size(); ++i)
    {
        std::stringstream ss; ss << "(sums) denominator degree: " << i;
        denSums[i]->dump("wtf.dot", true, ss.str().c_str());
    }
    
    ExpressionList numCoeffs;
    ExpressionList denCoeffs;
    combineSumsIntoCoefficients(&numCoeffs, numSums, variable);
    combineSumsIntoCoefficients(&denCoeffs, denSums, variable);
    
    e->dump("wtf.dot", true);

    for (std::size_t i = 0; i != numCoeffs.size(); ++i)
    {
        std::stringstream ss; ss << "(coeffs) numerator degree: " << i;
        numCoeffs[i]->dump("wtf.dot", true, ss.str().c_str());
    }
    for (std::size_t i = 0; i != denCoeffs.size(); ++i)
    {
        std::stringstream ss; ss << "(coeffs) denominator degree: " << i;
        denCoeffs[i]->dump("wtf.dot", true, ss.str().c_str());
    }

    return TFCoefficients();
}

// ----------------------------------------------------------------------------
tfp::TransferFunction<double>
TFManipulator::calculateTransferFunction(const TFCoefficients& tfe,
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
