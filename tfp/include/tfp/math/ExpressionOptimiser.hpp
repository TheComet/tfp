#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"

namespace tfp {

class Expression;

class TFP_PUBLIC_API ExpressionOptimiser
{
public:
    bool everything(Expression* e);
    bool constants(Expression* e);
    bool uselessOperations(Expression* e);
    bool simplify(Expression* e);

private:
    bool recursivelyCall(bool (ExpressionOptimiser::*optfunc)(Expression*), Expression* e);
    bool optimiseMultipleNegates(Expression* e);
    bool optimiseConstantExpressions(Expression* e);
    bool optimiseUselessAdditions(Expression* e);
    bool optimiseUselessSubtractions(Expression* e);
    bool optimiseUselessProducts(Expression* e);
    bool optimiseUselessDivisions(Expression* e);
    bool optimiseUselessExponents(Expression* e);
    bool collapseChainOfOperations(Expression* e);
    bool optimiseExponentiate(Expression* e);
    bool optimiseCombineExponents(Expression* e);
    bool optimiseAdditionsIntoProducts(Expression* e);
};

}
