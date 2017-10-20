#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
void Expression::optimise()
{
    bool mutated;
    do
    {
        mutated = false;
        mutated |= recursivelyCall(&Expression::optimiseMultipleNegates);
        mutated |= recursivelyCall(&Expression::optimiseConstantExpressions);
        mutated |= recursivelyCall(&Expression::optimiseUselessAdditions);
        mutated |= recursivelyCall(&Expression::optimiseUselessSubtractions);
        mutated |= recursivelyCall(&Expression::optimiseUselessProducts);
        mutated |= recursivelyCall(&Expression::optimiseUselessDivisions);
        mutated |= recursivelyCall(&Expression::optimiseUselessExponents);
        mutated |= recursivelyCall(&Expression::optimiseAdditionsIntoProducts);
        mutated |= recursivelyCall(&Expression::optimiseExponentiate);
        mutated |= recursivelyCall(&Expression::collapseChainOfOperations);
    } while(mutated);
}

// ----------------------------------------------------------------------------
bool Expression::checkParentConsistencies() const
{
    bool success = true;
    if (left())  success &= left()->checkParentConsistencies();
    if (right()) success &= right()->checkParentConsistencies();

    if (left() && left()->parent() != this)
    {
        std::cout << "Parent of expression is pointing somewhere else!" << std::endl;
        success = false;
    }
    if (right() && right()->parent() != this)
    {
        std::cout << "Parent of expression is pointing somewhere else!" << std::endl;
        success = false;
    }

    if (left() && left()->refs() != 1)
    {
        std::cout << "Refcount of sub-expression is not 1" << std::endl;
        success = false;
    }
    if (right() && right()->refs() != 1)
    {
        std::cout << "Refcount of sub-expression is not 1" << std::endl;
        success = false;
    }

    return success;
}
