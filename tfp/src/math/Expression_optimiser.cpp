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
    return false;  // TODO
}

// ----------------------------------------------------------------------------
bool Expression::optimiseExponentiate()
{
    if (right() == NULL || left() == NULL)
        return false;

    if (isOperation(op::mul) && left()->isSameAs(right()))
    {
        set(op::pow, left(), Expression::make(2.0));
        return true;
    }

    if (isOperation(op::pow) && parent() && parent()->isOperation(op::mul))
    {
        Expression* otherOperand = getOtherOperand();
        if (left()->isSameAs(otherOperand))
        {
            parent()->set(op::pow,
                this,
                Expression::make(op::add,
                    right(),
                    Expression::make(1.0)));
            return true;
        }
    }

    return false;  // TODO
}

// ----------------------------------------------------------------------------
bool Expression::collapseChainOfOperations()
{
    bool ret = false;

    if (isOperation(op::mul) && (left()->type() == CONSTANT || right()->type() == CONSTANT))
    {
        double product = left()->type() == CONSTANT ? left()->value() : right()->value();
        Expression* e = parent();
        while (e != NULL && e->isOperation(op::mul))
        {
            if (e->left()->type() == CONSTANT || e->right()->type() == CONSTANT)
            {
                if (e->left()->type() == CONSTANT)
                {
                    product *= e->left()->value();
                    e->set(e->right());
                    ret = true;
                }
                else
                {
                    product *= e->right()->value();
                    e->set(e->left());
                    ret = true;
                }
            }
            e = e->parent();
        }

        if (e->left()->type() == CONSTANT)
            e->left()->set(product);
        else
            e->right()->set(product);

        return ret;
    }

    if (isOperation(op::add) && (left()->type() == CONSTANT || right()->type() == CONSTANT))
    {
        double sum = left()->type() == CONSTANT ? left()->value() : right()->value();
        Expression* e = parent();
        while (e != NULL && e->isOperation(op::mul))
        {
            if (e->left()->type() == CONSTANT || e->right()->type() == CONSTANT)
            {
                if (e->left()->type() == CONSTANT)
                {
                    sum += e->left()->value();
                    e->set(e->right());
                    ret = true;
                }
                else
                {
                    sum += e->right()->value();
                    e->set(e->left());
                    ret = true;
                }
            }
            e = e->parent();
        }

        if (e->left()->type() == CONSTANT)
            e->left()->set(sum);
        else
            e->right()->set(sum);

        return ret;
    }

    return ret;
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
