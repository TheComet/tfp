#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool Expression::optimiseMultipleNegates()
{
    if (type() != FUNCTION1) return false;
    if (isOperation(op::negate) == false || hasRHSOperation(op::negate) == false)
        return false;

    right()->right()->collapseIntoParent();
    right()->collapseIntoParent();
    return true;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseUselessAdditions()
{
    if (isOperation(op::add) == false)
        return false;
    
    if (left()->type() == CONSTANT && left()->value() == 0.0)
    {
        right()->collapseIntoParent();
        return true;
    }
    if (right()->type() == CONSTANT && right()->value() == 0.0)
    {
        left()->collapseIntoParent();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseUselessSubtractions()
{
    if (isOperation(op::sub) == false)
        return false;

    if (left()->type() == CONSTANT && left()->value() == 0.0)
    {
        set(op::negate, right());
        return true;
    }
    if (right()->type() == CONSTANT && right()->value() == 0.0)
    {
        left()->collapseIntoParent();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseUselessProducts()
{
    if (isOperation(op::mul) == false)
        return false;
    
    if (left()->type() == CONSTANT)
    {
        if (left()->value() == 1.0)
        {
            right()->collapseIntoParent();
            return true;
        }
        if (left()->value() == -1.0)
        {
            set(op::negate, right());
            return true;;
        }
    }
    
    if (right()->type() == CONSTANT)
    {
        if (right()->value() == 1.0)
        {
            left()->collapseIntoParent();
            return true;
        }
        else if (right()->value() == -1.0)
        {
            set(op::negate, left());
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseUselessDivisions()
{
    if (isOperation(op::div) == false)
        return false;

    if (right()->type() == CONSTANT)
    {
        if (right()->value() == 1.0)
        {
            left()->collapseIntoParent();
            return true;
        }
        if (right()->value() == -1.0)
        {
            set(op::negate, left());
            return true;
        }
    }

    if (left()->isSameAs(right()))
    {
        set(1.0);
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseUselessExponents()
{
    if (isOperation(op::pow) == false)
        return false;
    
    if (right()->type() == CONSTANT && right()->value() == 1.0)
    {
        left()->collapseIntoParent();
        return true;
    }
    return false;
}
