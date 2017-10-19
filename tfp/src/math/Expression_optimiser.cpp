#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
void Expression::optimise()
{
    if (left())  left()->optimise();
    if (right()) right()->optimise();

    while (true)
    {

    /*
     * Eliminates double negates.
     */
    if (type() == FUNCTION1)
    {
        if (isOperation(op::negate) && hasRHSOperation(op::negate))
        {
            set(right()->right());
            continue;
        }

        if (right()->type() == CONSTANT)
        {
            set(evaluate());
            continue;
        }
    }

    // Anything beyond this point must have two operands.
    if (type() != FUNCTION2)
        return;

    /*
     * Evaluate any constant expressions.
     */
    if (left()->type() == CONSTANT && right()->type() == CONSTANT)
    {
        set(evaluate());
        continue;
    }

    /*
     * Eliminates addition with 0
     */
    if (isOperation(op::add))
    {
        if (left()->type() == CONSTANT && left()->value() == 0.0)
        {
            set(right());
            continue;
        }
        else if (right()->type() == CONSTANT && right()->value() == 0.0)
        {
            set(left());
            continue;
        }
    }

    /*
     * Eliminates subtraction by 0 and turns subtraction from 0 into a negate.
     */
    else if (isOperation(op::sub))
    {
        if (left()->type() == CONSTANT && left()->value() == 0.0)
        {
            set(op::negate, right());
            continue;
        }
        else if (right()->type() == CONSTANT && right()->value() == 0.0)
        {
            set(left());
            continue;
        }
    }

    /*
     * Eliminates multiplication by 1
     */
    else if (isOperation(op::mul))
    {
        if (left()->type() == CONSTANT)
        {
            if (left()->value() == 1.0)
            {
                set(right());
                continue;
            }
            else if (left()->value() == -1.0)
            {
                set(op::negate, right());
                continue;
            }
        }
        else if (right()->type() == CONSTANT)
        {
            if (right()->value() == 1.0)
            {
                set(left());
                continue;
            }
            else if (right()->value() == -1.0)
            {
                set(op::negate, left());
                continue;
            }
        }
    }

    /*
     * Eliminates division by 1
     */
    else if (isOperation(op::div))
    {
        if (right()->type() == CONSTANT)
        {
            if (right()->value() == 1.0)
            {
                set(left());
                continue;
            }
            else if (right()->value() == -1.0)
            {
                set(op::negate, left());
                continue;
            }
        }
    }

    /*
     * Eliminates raising to the power of 1
     */
    else if (isOperation(op::pow))
    {
        if (right()->type() == CONSTANT && right()->value() == 1.0)
        {
            set(left());
            continue;
        }
    }

    break;
    }
}
