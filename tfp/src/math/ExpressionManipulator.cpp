#include "tfp/math/ExpressionManipulators.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Tears.hpp"
#include <stdarg.h>

using namespace tfp;

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::makeError(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    g_tears.vcry(fmt, va);
    va_end(va);

    return ERROR;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::combineResults(Result a, Result b)
{
    /*
     * Same as writing:
     *
     *   if (a == MODIFIED || b == MODIFIED)
     *       return MODIFIED;
     *   if (a == ERROR || b == ERROR)
     *       return ERROR;
     *   return UNMODIFIED;
     */
    return a == b ? a : UNMODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result
ExpressionManipulator::runUntilUnmodified(Result (*optfunc)(Expression*,const char*),
                                          Expression* e, const char* variable)
{
    Result result, newResult;

    result = newResult = recursivelyCall(optfunc, e, variable);
    while (newResult == MODIFIED)
    {
        result = combineResults(result, newResult);
        newResult = recursivelyCall(optfunc, e, variable);
    }

    return result;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result
ExpressionManipulator::recursivelyCall(Result (*optfunc)(Expression*,const char*),
                                       Expression* e, const char* variable, bool* hasVariable)
{
    Result lhsResult, rhsResult, thisResult;
    bool childHadVariable = false;

    if (e->left())
    {
        if ((lhsResult = recursivelyCall(optfunc, e->left(), variable, &childHadVariable)) == ERROR)
            return ERROR;
    }
    else
        lhsResult = UNMODIFIED;

    if (e->right())
    {
        if ((rhsResult = recursivelyCall(optfunc, e->right(), variable, &childHadVariable)) == ERROR)
            return ERROR;
    }
    else
        rhsResult = UNMODIFIED;

    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    if (hasVariable != NULL)
        *hasVariable |= childHadVariable;
    if (childHadVariable == false && e->hasVariable(variable) == false)
        return UNMODIFIED;
    if (hasVariable != NULL)
        *hasVariable = true;

    if ((thisResult = (*optfunc)(e, variable)) == ERROR)
        return ERROR;

    if (lhsResult == MODIFIED || rhsResult == MODIFIED || thisResult == MODIFIED)
        return MODIFIED;
    return UNMODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::enforceProductLHS(Expression* e, const char* variable)
{
    if (e->isOperation(op::mul))
    {
        if (e->right() && e->right()->hasVariable(variable))
        {
            e->swapOperands();
            return MODIFIED;
        }
    }
    return UNMODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::enforceConstantExponent(Expression* e, const char* variable)
{
    /*
     * Performs the following transformations:
     *    s    --> s^1
     * If s is raised to a non-constant (e.g. s^a or (s+2)^a) abort.
     */
    if (e->isOperation(op::pow))
    {
        if (e->right()->type() != Expression::CONSTANT)
            return makeError("Variable must be raised to a constant expression.");
        return UNMODIFIED;
    }

    if (e->hasVariable(variable))
    {
        if (e->parent() && e->parent()->isOperation(op::pow))
            return UNMODIFIED;

        e->set(op::pow, e, Expression::make(1.0));
        return MODIFIED;
    }

    return UNMODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::expandConstantExponentsIntoProducts(Expression* e, const char* variable)
{
    /*
     * Performs the following transformations:
     *   s^c  (c=const)  --> s*s*s*...
     *   s^-c (c=const)  --> (s*s*s*...)^-1
     */
    if (e->isOperation(op::pow) == false)
        return UNMODIFIED;

    if (e->right()->type() != Expression::CONSTANT ||
            std::floor(e->right()->value()) != e->right()->value())
            return makeError("Variable must be raised to a constant expression. Can't expand exponents into products.");

    int exponent = (int)e->right()->value();

    if (exponent == 0)
    {
        e->set(1.0);
        return MODIFIED;
    }

    if (exponent == 1 || exponent == -1)
    {
        return UNMODIFIED;
    }

    Expression* toFactor = e->left();
    e->set(op::mul, toFactor, toFactor->clone());
    int expandTimes = exponent < 0 ? -exponent : exponent;
    for (int i = 2; i != expandTimes; ++i)
    {
        e->set(op::mul, e, toFactor->clone());
    }

    if (exponent < 0)
    {
        e->set(op::pow, e, Expression::make(-1.0));
    }

    return MODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result
ExpressionManipulator::factorNegativeExponents(Expression* e, const char* variable)
{
    /*
     * Does the following manipulations:
     *    x + ys^-c (c=const)   ->   s^-c(xs^c + y)
     */
    if (e->isOperation(op::pow) == false)
        return UNMODIFIED;
    if (e->right()->type() != Expression::CONSTANT)
            return makeError("Variable must be raised to a constant expression. Can't factor negative exponents.");

    // Nothing to do
    if (e->right()->value() >= 0)
        return UNMODIFIED;

    // This operation only makes sense if there is an add above us
    Expression* add = e;
    Expression* firstNonAddOpInChain = e;
    while ((add = add->parent()) != NULL)
    {
        if (add->isOperation(op::add))
            break;
        firstNonAddOpInChain = add;
    }
    if (add == NULL)
        return UNMODIFIED;

    // Maybe there's more
    while (add->parent() && add->parent()->isOperation(op::add))
        add = add->parent();

    /*
     * We've reached the highest add operator, now factor in s^c to cancel out
     * s^-c.
     *
     * From above, while searching for the first add operator above us, we kept
     * track of the first non-add operator. This is because factorIn would use
     * this to insert the factor, so we have to tell it to ignore that node.
     */
    Expression* toFactorIn = e->clone();
    if (toFactorIn->right()->type() == Expression::CONSTANT)
        toFactorIn->right()->set(-toFactorIn->right()->value());
    else
        toFactorIn->right()->set(op::mul, toFactorIn->right(), Expression::make(-1.0));

    factorIn(add, toFactorIn, firstNonAddOpInChain);

    // We get moved out of the brackets and set to 1.0
    add->set(op::mul, add, e->unlinkExchange(Expression::make(1.0)));

    return MODIFIED;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::factorIn(Expression* e, Expression* toFactor, const Expression* ignore)
{
    if (e == ignore)
        return UNMODIFIED;

    if (e->isOperation(op::add))
    {
        return combineResults(
            factorIn(e->left(), toFactor, ignore),
            factorIn(e->right(), toFactor, ignore)
        );
    }
    else
    {
        e->set(op::mul, e, toFactor->clone());
        return MODIFIED;
    }
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::eliminateDivisionsAndSubtractions(Expression* e, const char* variable)
{
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

    Result result = UNMODIFIED;
    for (int i = 0; i != 2; ++i)
    {
        if (e->isOperation(ops[i].outer) == false)
            continue;

        if (e->hasRHSOperation(ops[i].inner))
        {
            Expression* toNegate = e->right()->right();
            e->set(ops[i].outerInv, e->left(), e->right());
            if (toNegate->type() == Expression::CONSTANT)
                toNegate->set(-toNegate->value());
            else
                toNegate->set(op::mul, toNegate, Expression::make(-1.0));

        }
        else
        {
            e->set(ops[i].outerInv,
                e->left(),
                Expression::make(ops[i].inner,
                    e->right(),
                    Expression::make(-1)));
        }
        result = MODIFIED;
    }

    return result;
}

// ----------------------------------------------------------------------------
ExpressionManipulator::Result ExpressionManipulator::expand(Expression* e, const char* variable)
{
    /*
     * Performs manipulations such as
     *   (a+s)(b+s)  -->  ab + as + sb + ss
     *   a(s+4)      -->  as + 4a
     *
     * Does not handle expressions with powers.
     */
    if (e->parent() == NULL)
        return UNMODIFIED;
    if (e->isOperation(op::add) == false || e->parent()->isOperation(op::mul) == false)
        return UNMODIFIED;

    Expression* factorInOther = e->getOtherOperand();
    Expression* factorInThis = factorInOther->clone();
    e->parent()->set(op::add, e, Expression::make(op::mul, factorInOther, e->right()));
    e->set(op::mul, factorInThis, e->left());

    return combineResults(MODIFIED, combineResults(
        recursivelyCall(&expand, factorInOther, variable),
        recursivelyCall(&expand, factorInThis, variable)
    ));
}
