#include "tfp/math/Expression.hpp"

using namespace tfp;

#define RECURSE(method)                            \
        bool mutated = false;                      \
        if (left())  mutated |= left()->method();  \
        if (right()) mutated |= right()->method();

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
bool Expression::optimiseMultipleNegates()
{
    if (type() != FUNCTION1) return false;
    if (isOperation(op::negate) == false || hasRHSOperation(op::negate) == false)
        return false;

    set(right()->right());
    return true;
}

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
bool Expression::optimiseUselessAdditions()
{
    if (isOperation(op::add) == false)
        return false;
    
    if (left()->type() == CONSTANT && left()->value() == 0.0)
    {
        set(right());
        return true;
    }
    if (right()->type() == CONSTANT && right()->value() == 0.0)
    {
        set(left());
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
        set(left());
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
            set(right());
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
            set(left());
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
            set(left());
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
        set(left());
        return true;
    }
    return false;
}

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
        Reference<Expression>& nonConstant = left()->type() == CONSTANT ? right_ : left_;

        // Travel up the chain of op::muls...
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

        nonConstant = Expression::make(op::mul, nonConstant, Expression::make(2.0));
        same->parent()->set(same->getOtherOperand());
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
        Reference<Expression>& nonConstant = left()->type() == CONSTANT ? right_ : left_;

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

        nonConstant = Expression::make(op::pow, nonConstant, Expression::make(2.0));
        same->parent()->set(same->getOtherOperand());
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
                e->set(e->right());
                return true;
            }
            if (left()->isSameAs(e->right()))
            {
                right_ = Expression::make(op::add, right(), Expression::make(1.0));
                e->set(e->left());
                return true;
            }
        }
    }

    return false;  // TODO
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
        e->set(collapse);
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

        Expression* lhs = left()->type() == CONSTANT ? left() : right();
        Expression* rhs = e->left()->type() == CONSTANT ? e->left() : e->right();
        Expression* collapse = e->left()->type() == CONSTANT ? e->right() : e->left();

        lhs->set(lhs->value() * rhs->value());
        e->set(collapse);
        return true;
    }

    return false;
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
