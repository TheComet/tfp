#include "tfp/math/ExpressionManipulators.hpp"
#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <cstring>

using namespace tfp;

// ----------------------------------------------------------------------------
bool TFManipulator::manipulateIntoRationalFunction(Expression* e, const char* variable)
{
    /*
     * Create the "split" operator, i.e. this is the expression that splits the
     * numerator from the denominator. By default, use the last division
     * operator we can find in the tree. If none exists, create a div operator
     * that has no effect.
     *
     * XXX Does this actually produce the correct results?
     */
    Expression* split = e->find(op::div);
    if (split == NULL)
        split = Expression::make(op::div, e, Expression::make(1.0));

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
    bool weAreDone = false;
    ExpressionOptimiser optimise;
    while (true)
    {
        recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split->right(), variable);
        recursivelyCall(&TFManipulator::expand, split->right(), variable);
        optimise.constants(split->right());
        optimise.simplify(split->right());

        recursivelyCall(&TFManipulator::factorNegativeExponents, split->right(), variable);

        bool didFactorStuffOut = false;
        while (true)
        {
            // XXX This can potentially get stuck in an endless loop if the negative exponent cannot be
            // factored out.
            Expression* expToFactorOut = split->right()->findOpWithNegativeRHS(op::pow);
            if (expToFactorOut == NULL)
                break;

            Expression* e = expToFactorOut->parent();
            while (e->isOperation(op::mul))
                e = e->parent();
            if (e != split)
                continue;

            expToFactorOut->right()->set(-expToFactorOut->right()->value());
            factorIn(split->left(), expToFactorOut);
            expToFactorOut->getOtherOperand()->collapseIntoParent();
            didFactorStuffOut = true;
        }

        if (weAreDone)
            break;

        if (didFactorStuffOut == false)
            weAreDone = true;
    }

    while (optimise.uselessOperations(split->left()) |
           recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split->left(), variable) |
           recursivelyCall(&TFManipulator::expand, split->left(), variable))
    {}

    optimise.everything(e);

    return false;
}

// ----------------------------------------------------------------------------
typedef std::pair< int, Reference<Expression> > CoeffExp;
typedef std::vector<CoeffExp> UnsortedCoeffs;
static bool coeffsPow(UnsortedCoeffs* coeffs, Expression* e, const char* variable)
{
    if (e->isOperation(op::pow))
    {
        return coeffsPow(coeffs, e->right(), variable) |
               coeffsPow(coeffs, e->left(), variable);
    }
    else
    {
        if (e->hasVariable(variable))
        {
            CoeffExp coeff;
            if (e->parent()->isOperation(op::pow))
                coeff.first = (int)e->parent()->right()->value();
            else
                coeff.first = 1;

            e = e->parent()->isOperation(op::pow) ? e->parent() : e;
            Expression* top = e;
            while (top->parent() && top->parent()->isOperation(op::mul))
                top = top->parent();

            coeff.second = top;
            e->getOtherOperand()->collapseIntoParent();
            top->getOtherOperand()->collapseIntoParent();
            coeffs->push_back(coeff);
            return true;
        }
    }
    return false;
}
static bool coeffsMul(UnsortedCoeffs* coeffs, Expression* e, const char* variable)
{
    if (e->isOperation(op::mul))
    {
        return coeffsMul(coeffs, e->right(), variable) ||
               coeffsMul(coeffs, e->left(), variable);
    }
    else
    {
        return coeffsPow(coeffs, e, variable);
    }
}
static bool coeffsAdd(UnsortedCoeffs* coeffs, Expression* e, const char* variable)
{
    if (e->isOperation(op::add))
    {
        return coeffsAdd(coeffs, e->right(), variable) ||
               coeffsAdd(coeffs, e->left(), variable);
    }
    else
    {
        return coeffsMul(coeffs, e, variable);
    }
}

TFManipulator::TFCoefficients
TFManipulator::calculateTransferFunctionCoefficients(Expression* e, const char* variable)
{
    manipulateIntoRationalFunction(e, variable);

    UnsortedCoeffs num;
    UnsortedCoeffs den;
    while (coeffsAdd(&num, e->left(), variable)) {}
    while (coeffsAdd(&den, e->right(), variable)) {}

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
