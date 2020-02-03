#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"

namespace tfp {

class Expression;

class TFP_PUBLIC_API ExpressionOptimiser
{
public:
    static bool everything(Expression* e);
    static bool constants(Expression* e);
    static bool uselessOperations(Expression* e);
    static bool simplify(Expression* e);

private:
    static bool recursivelyCall(bool (*optfunc)(Expression*), Expression* e);

    // UselessOperations
    static bool removeMultipleNegates(Expression* e);
    static bool removeUselessAdditions(Expression* e);
    static bool removeUselessSubtractions(Expression* e);
    static bool removeUselessProducts(Expression* e);
    static bool removeUselessDivisions(Expression* e);
    static bool removeUselessExponents(Expression* e);

    // Simplifications
    static bool simplifySums(Expression* e);
    static bool simplifyProducts(Expression* e);
    static bool simplifyExponents(Expression* e);

    // Constants
    static bool evaluateConstantExpressions(Expression* e);
    static bool combineConstants(Expression* e);
};

}
