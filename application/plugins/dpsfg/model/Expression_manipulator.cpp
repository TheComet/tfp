#include "model/Expression.hpp"
#include <cstring>

using namespace dpsfg;

// ----------------------------------------------------------------------------
static bool variableIsChildOf(Expression* e, const char* variable)
{
    if (e->type() == Expression::VARIABLE && strcmp(e->name(), variable) == 0) return true;
    if (e->left()  && variableIsChildOf(e->left(),  variable)) return true;
    if (e->right() && variableIsChildOf(e->right(), variable)) return true;
    return false;
}

// ----------------------------------------------------------------------------
void Expression::reorderProducts(const char* variable)
{
    if (isOperation(op::mul))
    {
        if (right() && right()->type() == VARIABLE && strcmp(right()->name(), variable) == 0)
        {
            tfp::Reference<Expression> tmp = right_;
            right_ = left_;
            left_ = tmp;
        }
    }

    if (left())  left()->reorderProducts(variable);
    if (right()) right()->reorderProducts(variable);
}

// ----------------------------------------------------------------------------
bool Expression::eliminateDivisionsAndSubtractions(const char* variable)
{
    /*
     * Only manipulate branches that are an ancestor of an expression with the
     * specified variable.
     */
    bool weMatter = false;
    if (left())  weMatter |= left()->eliminateDivisionsAndSubtractions(variable);
    if (right()) weMatter |= right()->eliminateDivisionsAndSubtractions(variable);
    if (weMatter == false && hasVariable(variable) == false)
        return false;

    /*
     * Performs the following transformations:
     *   a/s    -->  as^-1
     *   a/s^x  -->  as^-x
     *   a-s    -->  a+s*(-1)
     *   a-sx   -->  a+s*(-x)
     * This makes the tree easier to work with for later stages, because
     * the assumption that no division or subtraction operators exist can be
     * made.
     */
    static struct { op::Op2 outer; op::Op2 outerInv; op::Op2 inner; } ops[2] = {
        { op::div, op::mul, op::pow },
        { op::sub, op::add, op::mul }
    };

    for (int i = 0; i != 2; ++i)
    {
        if (isOperation(ops[i].outer))
        {
            if (hasRHSOperation(ops[i].inner))
            {
                Expression* toNegate = right()->right();
                set(ops[i].outerInv, left(), right());
                if (toNegate->type() == CONSTANT)
                    toNegate->set(-toNegate->value());
                else
                    toNegate->set(op::negate, toNegate->clone());
            }
            else
            {
                set(ops[i].outerInv,
                    left(),
                    Expression::make(ops[i].inner,
                                     right(),
                                     Expression::make(-1)));
            }
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
bool Expression::manipulateIntoRationalFunction(const char* variable)
{
    return false;
}
