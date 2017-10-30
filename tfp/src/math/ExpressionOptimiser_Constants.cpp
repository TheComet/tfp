#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::evaluateConstantExpressions(Expression* e)
{
    if (e->type() == Expression::FUNCTION1 &&
        e->right()->type() == Expression::CONSTANT)
    {
        e->set(e->evaluate());
        e->root()->dump("wtf.dot", true, "evaluateConstantExpressions 1");
        return true;
    }

    if (e->type() == Expression::FUNCTION2 &&
        e->left()->type() == Expression::CONSTANT &&
        e->right()->type() == Expression::CONSTANT)
    {
        e->set(e->evaluate());
        e->root()->dump("wtf.dot", true, "evaluateConstantExpressions 2");
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::combineConstants(Expression* e)
{
    if (e->type() != Expression::FUNCTION2)
        return false;
    if (e->left()->type() != Expression::CONSTANT &&
        e->right()->type() != Expression::CONSTANT)  // Need at least one constant operand
        return false;

    if (e->isOperation(op::add))
    {
        // Travel up the chain of op::adds...
        Expression* top;
        for (top = e; top->parent() != NULL && top->parent()->isOperation(op::add);)
        {
            top = top->parent();
            if (top->left()->type() == Expression::CONSTANT ||
                top->right()->type() == Expression::CONSTANT)
                break;
        }
        if (top == e)
            return false;

        // And search downwards again for any expression node that has a constant
        Expression* same = top->findLorR(op::add, Expression::CONSTANT, e);
        if (same == NULL)
            return false;

        Expression* lhs      = e->left()->type()    == Expression::CONSTANT ? e->left()     : e->right();
        Expression* rhs      = same->left()->type() == Expression::CONSTANT ? same->left()  : same->right();
        Expression* collapse = same->left()->type() == Expression::CONSTANT ? same->right() : same->left();

        lhs->set(lhs->value() + rhs->value());
        collapse->collapseIntoParent();
        e->root()->dump("wtf.dot", true, "combineConstants 1");
        return true;
    }

    if (e->isOperation(op::mul))
    {
        // Travel up the chain of op::adds...
        Expression* top;
        for (top = e; top->parent() != NULL && top->parent()->isOperation(op::mul);)
        {
            top = top->parent();
            if (top->left()->type() == Expression::CONSTANT ||
                top->right()->type() == Expression::CONSTANT)
                break;
        }
        if (top == e)
            return false;

        // And search downwards again for any expression node that has a constant
        Expression* same = top->findLorR(op::mul, Expression::CONSTANT, e);
        if (same == NULL)
            return false;

        Expression* lhs      = e->left()->type()    == Expression::CONSTANT ? e->left()     : e->right();
        Expression* rhs      = same->left()->type() == Expression::CONSTANT ? same->left()  : same->right();
        Expression* collapse = same->left()->type() == Expression::CONSTANT ? same->right() : same->left();

        lhs->set(lhs->value() * rhs->value());
        collapse->collapseIntoParent();
        e->root()->dump("wtf.dot", true, "combineConstants 2");
        return true;
    }

    return false;
}
