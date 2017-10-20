#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool Expression::optimiseConstantExpressions()
{
    if (type() == FUNCTION1 && right()->type() == CONSTANT)
    {
        set(evaluate());
        return true;
    }

    // Anything beyond this point must have two operands.
    if (type() == FUNCTION2 && left()->type() == CONSTANT && right()->type() == CONSTANT)
    {
        set(evaluate());
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::collapseChainOfOperations()
{
    if (left() && left()->type() != CONSTANT && right() && right()->type() != CONSTANT)
        return false;

    if (isOperation(op::add))
    {
        // Travel up the chain of op::adds...
        Expression* e;
        for (e = this; e->parent() != NULL && e->parent()->isOperation(op::add);)
        {
            e = e->parent();
            if (e->left()->type() == CONSTANT || e->right()->type() == CONSTANT)
                break;
        }
        if (e == this)
            return false;

        // And search downwards again for any expression node that has a constant
        e = e->findLorR(op::add, CONSTANT, this);
        if (e == NULL)
            return false;

        Expression* lhs = left()->type() == CONSTANT ? left() : right();
        Expression* rhs = e->left()->type() == CONSTANT ? e->left() : e->right();
        Expression* collapse = e->left()->type() == CONSTANT ? e->right() : e->left();

        lhs->set(lhs->value() + rhs->value());
		collapse->collapseIntoParent();
        return true;
    }

    if (isOperation(op::mul))
    {
        // Travel up the chain of op::adds...
        Expression* e;
        for (e = this; e->parent() != NULL && e->parent()->isOperation(op::mul);)
        {
            e = e->parent();
            if (e->left()->type() == CONSTANT || e->right()->type() == CONSTANT)
                break;
        }
        if (e == this)
            return false;

        // And search downwards again for any expression node that has a constant
        e = e->findLorR(op::mul, CONSTANT, this);
        if (e == NULL)
            return false;

        Expression* lhs      = left()->type() == CONSTANT ? left() : right();
        Expression* rhs      = e->left()->type() == CONSTANT ? e->left() : e->right();
        Expression* collapse = e->left()->type() == CONSTANT ? e->right() : e->left();

        lhs->set(lhs->value() * rhs->value());
		collapse->collapseIntoParent();
        return true;
    }

    return false;
}
