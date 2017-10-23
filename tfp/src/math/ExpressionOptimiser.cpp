#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::everything(Expression* e)
{
    bool mutated;
    bool overallMutated = false;
    do
    {
        mutated = false;
        mutated |= constants(e);
        mutated |= uselessOperations(e);
        mutated |= simplify(e);

        if (mutated)
            overallMutated = true;
    } while (mutated);
    return overallMutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::constants(Expression* e)
{
    bool mutated;
    bool overallMutated = false;
    do
    {
        mutated = false;
        mutated |= recursivelyCall(&ExpressionOptimiser::evaluateConstantExpressions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::combineConstants, e);

        if (mutated)
            overallMutated = true;
    } while (mutated);
    return overallMutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::uselessOperations(Expression* e)
{
    bool mutated;
    bool overallMutated = false;
    do
    {
        mutated = false;
        mutated |= recursivelyCall(&ExpressionOptimiser::removeMultipleNegates, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::removeUselessAdditions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::removeUselessSubtractions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::removeUselessProducts, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::removeUselessDivisions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::removeUselessExponents, e);

        if (mutated)
            overallMutated = true;
    } while (mutated);
    return overallMutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::simplify(Expression* e)
{
    bool mutated;
    bool overallMutated = false;
    do
    {
        mutated = false;
        mutated |= recursivelyCall(&ExpressionOptimiser::simplifySums, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::simplifyProducts, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::simplifyExponents, e);

        if (mutated)
            overallMutated = true;
    } while (mutated);
    return overallMutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::recursivelyCall(bool (ExpressionOptimiser::*optfunc)(Expression*), Expression* e)
{
    WeakReference<Expression> self(e);

    bool mutated = false;
    if (e->left() && recursivelyCall(optfunc, e->left()))
    {
        mutated = true;
        if (self.expired())
            return true;
    }
    if (e->right() && recursivelyCall(optfunc, e->right()))
    {
        mutated = true;
        if (self.expired())
            return true;
    }
    return mutated | (this->*optfunc)(e);
}
