#include "tfp/math/ExpressionManipulators.hpp"
#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <cstring>

using namespace tfp;

// ----------------------------------------------------------------------------
ExpressionManipulator::Result TFManipulator::factorNegativeExponentsToNumerator(Expression* e, Expression* numerator, const char* variable)
{
    if (e->isOperation(op::mul))
    {
        // Return if an error occurred, or if a branch was modified
        Result lhsResult, rhsResult;
        if ((lhsResult = factorNegativeExponentsToNumerator(e->left(), numerator, variable)) != UNMODIFIED)
            return lhsResult;
        if ((rhsResult = factorNegativeExponentsToNumerator(e->right(), numerator, variable)) != UNMODIFIED)
            return rhsResult;
        return combineResults(lhsResult, rhsResult);
    }

    if (e->isOperation(op::pow) == false)
        return UNMODIFIED;

    if (e->right()->type() != Expression::CONSTANT || e->right()->value() >= 0.0)
        return UNMODIFIED;

    e->right()->set(-e->right()->value());
    factorIn(numerator, e);
    e->getOtherOperand()->collapseIntoParent();
    return MODIFIED;
}

// ----------------------------------------------------------------------------
inline bool logically_equal(double a, double b, double error_factor=1.0)
{
  return a==b ||
    std::abs(a-b)<std::abs(std::min(a,b))*std::numeric_limits<double>::epsilon()*
                  error_factor;
}
ExpressionManipulator::Result TFManipulator::manipulateIntoRationalFunction(Expression* e, const char* variable)
{
    Result result;
    e->dump("wtf.dot");

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
    result = combineResults(
        runUntilUnmodified(&eliminateDivisionsAndSubtractions, split->left(), variable),
        runUntilUnmodified(&eliminateDivisionsAndSubtractions, split->right(), variable)
    );

    /*
     * All op::pow operations with our variable on the LHS need to have a
     * constant RHS. If not, error out, because such an expression cannot be
     * reduced to a rational function.
     */
    while (true)
    {
        e->dump("wtf.dot", true);
        runUntilUnmodified(&expandConstantExponentsIntoProducts, split->right(), variable);
        e->dump("wtf.dot", true);
        runUntilUnmodified(&expand, split->right(), variable);
        e->dump("wtf.dot", true);
        ExpressionOptimiser::constants(split->right());
        ExpressionOptimiser::uselessOperations(split->right());

        runUntilUnmodified(&factorNegativeExponents, split->right(), variable);
        e->dump("wtf.dot", true);

        bool needToContinue = false;
        while (factorNegativeExponentsToNumerator(split->right(), split->left(), variable) == MODIFIED)
        {
            needToContinue = true;
        }
        if (needToContinue)
            continue;

        break;
    }

    while (ExpressionOptimiser::uselessOperations(split->left()) |
           runUntilUnmodified(&expandConstantExponentsIntoProducts, split, variable) |
           runUntilUnmodified(&expand, split, variable))
    {}

    return result;
}

// ----------------------------------------------------------------------------
static void splitSums(TFManipulator::TFCoefficients::Coefficients* sums,
                      Expression* e)
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

static void resizeCoefficientList(TFManipulator::TFCoefficients::Coefficients* coeffs,
                                  std::size_t size)
{
    while (coeffs->size() < size)
        coeffs->push_back(Expression::make(0.0));
}

static ExpressionManipulator::Result combineSumsIntoCoefficients(TFManipulator::TFCoefficients::Coefficients* coeffs,
                                             const TFManipulator::TFCoefficients::Coefficients& sums,
                                             const char* variable)
{
    for (TFManipulator::TFCoefficients::Coefficients::const_iterator it = sums.begin(); it != sums.end(); ++it)
    {
        int order = 0;

        // Determine coefficient order by finding the variable we're a function of and checking its exponent.
        Expression* var = (*it)->find(variable);
        if (var != NULL)
        {
            if (var->parent() && var->parent()->isOperation(op::pow))
            {
                if (var->parent()->right()->type() != Expression::CONSTANT)
                    return ExpressionManipulator::makeError("Variable needs to be raised to a constant exponent expression.");
                order = (int)var->parent()->right()->value();

                // Eliminate variable and power expression so we are left with the coefficient only
                if (var->parent()->parent())
                    var->parent()->getOtherOperand()->collapseIntoParent();
                else
                    var->parent()->set(1.0);
            }
            else
            {
                // No exponent is the same as an exponent of 1
                order = 1;
                // Eliminate variable so we are left with the coefficient only
                if (var->parent())
                    var->getOtherOperand()->collapseIntoParent();
                else
                    var->set(1.0);
            }
        }

        resizeCoefficientList(coeffs, order + 1);
        (*coeffs)[order] = Expression::make(op::add, (*coeffs)[order], *it);
    }

    return ExpressionManipulator::MODIFIED;
}

TFManipulator::TFCoefficients
TFManipulator::calculateTransferFunctionCoefficients(Expression* e, const char* variable)
{
    manipulateIntoRationalFunction(e, variable);

    TFCoefficients::Coefficients numSums;
    TFCoefficients::Coefficients denSums;
    splitSums(&numSums, e->left());
    splitSums(&denSums, e->right());

    for (std::size_t i = 0; i != numSums.size(); ++i)
        numSums[i]->optimise();
    for (std::size_t i = 0; i != denSums.size(); ++i)
        denSums[i]->optimise();

    TFCoefficients coeffs;
    combineSumsIntoCoefficients(&coeffs.numerator, numSums, variable);
    combineSumsIntoCoefficients(&coeffs.denominator, denSums, variable);

    for (std::size_t i = 0; i != coeffs.numerator.size(); ++i)
        coeffs.numerator[i]->optimise();
    for (std::size_t i = 0; i != coeffs.denominator.size(); ++i)
        coeffs.denominator[i]->optimise();

    return coeffs;
}

// ----------------------------------------------------------------------------
TransferFunction
TFManipulator::calculateTransferFunction(const TFCoefficients& tfe,
                                         const VariableTable* vt)
{
    CoefficientPolynomial numerator(tfe.numerator.size());
    CoefficientPolynomial denominator(tfe.denominator.size());

    for (std::size_t i = 0; i != tfe.numerator.size(); ++i)
    {
        double value = tfe.numerator[i]->evaluate(vt);
        numerator.setCoefficient(i, value);
    }

    for (std::size_t i = 0; i != tfe.denominator.size(); ++i)
    {
        double value = tfe.denominator[i]->evaluate(vt);
        denominator.setCoefficient(i, value);
    }

    return TransferFunction(numerator, denominator);
}
