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
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseConstantExpressions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::collapseChainOfOperations, e);

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
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseMultipleNegates, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseUselessAdditions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseUselessSubtractions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseUselessProducts, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseUselessDivisions, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseUselessExponents, e);

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
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseAdditionsIntoProducts, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseExponentiate, e);
        mutated |= recursivelyCall(&ExpressionOptimiser::optimiseCombineExponents, e);

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
