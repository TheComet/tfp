#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool Expression::optimiseAdditionsIntoProducts()
{
    if (right() == NULL || left() == NULL)
        return false;

    if (isOperation(op::add))
    {
        /*
         * Choose one of our operands that is not constant. The constant
         * operands are handled in collapseChainOfOperations().
         * We will then search the tree for another node that matches this one.
         */
        Expression* nonConstant = left()->type() == CONSTANT ? right_ : left_;

        // Travel up the chain of op::adds...
        Expression* top;
        for (top = nonConstant; top->parent() != NULL && top->parent()->isOperation(op::add); top = top->parent())
        {}
        if (top == nonConstant)
            return false;

        // And search downwards again for any expression node that is the same
        // as one of our non-constant operands
        Expression* same = top->findSame(nonConstant);
        if (same == NULL)
            return false;

        // We've found two identical expressions being added together. That's
        // equivalent to multiplying one of them by 2.
        nonConstant->set(op::mul, nonConstant, Expression::make(2.0));
        same->getOtherOperand()->collapseIntoParent();
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
bool Expression::optimiseExponentiate()
{
    if (right() == NULL || left() == NULL)
        return false;

    if (isOperation(op::mul))
    {
        /*
         * Choose one of our operands that is not constant. The constant
         * operands are handled in collapseChainOfOperations().
         * We will then search the tree for another node that matches this one.
         */
        Expression* nonConstant = left()->type() == CONSTANT ? right_ : left_;

        // Travel up the chain of op::muls...
        Expression* top;
        for (top = nonConstant; top->parent() != NULL && top->parent()->isOperation(op::mul); top = top->parent())
        {}
        if (top == nonConstant)
            return false;

        // And search downwards again for any expression node that is the same
        // as one of our non-constant operands
        Expression* same = top->findSame(nonConstant);
        if (same == NULL)
            return false;

        nonConstant->set(op::pow, nonConstant, Expression::make(2.0));
        same->getOtherOperand()->collapseIntoParent();
        return true;
    }

    if (isOperation(op::pow))
    {
        // TODO move this into its own function, as the two above
        // TODO travel up chain, recurse down again
        // TODO write unit test that falsifies this.
        Expression* e = this;
        while ((e = e->parent()) != NULL && e->isOperation(op::mul))
        {
            if (left()->isSameAs(e->left()))
            {
                right_ = Expression::make(op::add, right(), Expression::make(1.0));
                e->right()->collapseIntoParent();
                return true;
            }
            if (left()->isSameAs(e->right()))
            {
                right()->set(op::add, right(), Expression::make(1.0));
                e->left()->collapseIntoParent();
                return true;
            }
        }
    }

    return false;  // TODO
}
