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
    split->dump("wtf.dot");
    bool weAreDone = false;
    ExpressionOptimiser optimise;
    while (true)
    {
        recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split->left(), variable);
        recursivelyCall(&TFManipulator::expand, split->left(), variable);
        recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, split->right(), variable);
        recursivelyCall(&TFManipulator::expand, split->right(), variable);
        split->dump("wtf.dot", true);
        optimise.everything(split->left());
        optimise.everything(split->right());
        split->dump("wtf.dot", true);

        recursivelyCall(&TFManipulator::factorNegativeExponents, split->right(), variable);
        split->dump("wtf.dot", true);

        bool didFactorStuffOut = false;
        while (true)
        {
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
            split->dump("wtf.dot", true);
            didFactorStuffOut = true;
        }

        if (weAreDone)
            break;

        if (didFactorStuffOut == false)
            weAreDone = true;
    }

    split->dump("wtf.dot", true);
    optimise.everything(split);
    split->dump("wtf.dot", true);

    return false;
}

// ----------------------------------------------------------------------------
TFManipulator::TFCoefficients
TFManipulator::calculateTransferFunctionCoefficients(Expression* e, const char* variable)
{
    manipulateIntoRationalFunction(e, variable);

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
