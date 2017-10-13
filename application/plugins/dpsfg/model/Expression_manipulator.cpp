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

void Expression::eliminateDivisionsAndSubtractions()
{
    /*
     * Performs the following transformations:
     *   a/s    -->  as^-1
     *   a/s^x  -->  as^-x
     *   a-s    -->  a+s*(-1)
     *   a-sx   -->  a+s*(-x)
     *   a-xs   -->  a+s*(-x)
     * This makes the tree easier to work with for later stages, because
     * the assumption that no division or subtraction operators exist can be
     * made.
     */
    static struct { op::Op2 op; op::Op2 precedes; op::Op2 inverse; } ops[2] = {
        { op::div, op::pow, op::mul },
        { op::sub, op::mul, op::add }
    };

    for (int i = 0; i != 2; ++i)
    {
        if (isOperation(ops[i].op))
        {
            if (hasRHSOperation(ops[i].precedes))
            {
                Expression* toNegate = right()->right();
                set(ops[i].inverse, left(), right());
                if (toNegate->type() == CONSTANT)
                    toNegate->set(-toNegate->value());
                else
                    toNegate->set(op::negate, toNegate->value());
            }
            else
            {
				set(ops[i].inverse,
                    left(),
                    Expression::make(ops[i].precedes,
                                     right(),
                                     Expression::make(-1)));
            }
        }
    }

    if (left()) left()->eliminateDivisionsAndSubtractions();
    if (right()) right()->eliminateDivisionsAndSubtractions();
}

// ----------------------------------------------------------------------------
bool Expression::manipulateIntoRationalFunction(const char* variable)
{
    return false;
}
