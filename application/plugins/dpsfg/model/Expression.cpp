#include "model/Expression.hpp"
#include "model/VariableTable.hpp"
#include <string.h>
#include <cassert>

using namespace dpsfg;

// ----------------------------------------------------------------------------
Expression::Expression() :
    parent_(NULL),
    type_(INVALID)
{
}

// ----------------------------------------------------------------------------
Expression::~Expression()
{
    reset();
}

// ----------------------------------------------------------------------------
Expression* Expression::make(const char* variableName)
{
    Expression* e = new Expression;
    e->set(variableName);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double value)
{
    Expression* e = new Expression;
    e->set(value);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(op::Op1 func, Expression* rhs)
{
    Expression* e = new Expression;
    e->set(func, rhs);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(op::Op2 func, Expression* lhs, Expression* rhs)
{
    Expression* e = new Expression;
    e->set(func, lhs, rhs);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::clone() const
{
    Expression* e = new Expression;
    e->set(this);
    return e;
}

// ----------------------------------------------------------------------------
void Expression::set(const char* variableName)
{
    reset();

    type_ = VARIABLE;
    name_ = (char*)malloc((sizeof(char) + 1) * strlen(variableName));
    strcpy(name_, variableName);
}

// ----------------------------------------------------------------------------
void Expression::set(double value)
{
    reset();

    type_ = CONSTANT;
    value_ = value;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op1 func, Expression* rhs)
{
    reset();

    type_ = FUNCTION1;
    op1_ = func;
    right_ = rhs;
    rhs->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op2 func, Expression* lhs, Expression* rhs)
{
    reset();

    type_ = FUNCTION2;
    op2_ = func;
    left_ = lhs;
    right_ = rhs;
    lhs->parent_ = this;
    rhs->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::set(const Expression* other)
{
    reset();

    type_ = other->type_;
    switch (type_)
    {
        case CONSTANT  : value_ = other->value_; break;
        case FUNCTION1 : op1_ = other->op1_; break;
        case FUNCTION2 : op2_ = other->op2_; break;
        case VARIABLE  :
            name_ = (char*)malloc((strlen(other->name_) + 1) * sizeof(char));
            strcpy(name_, other->name_);
            break;
        case INVALID: break;
    }
}

// ----------------------------------------------------------------------------
void Expression::reset()
{
    if (type_ == VARIABLE)
        free(name_);
    type_ = INVALID;
}

// ----------------------------------------------------------------------------
static void addVariableToTable(VariableTable* vt, const Expression* e)
{
    if (e == NULL)
        return;

    addVariableToTable(vt, e->left());
    addVariableToTable(vt, e->right());

    if (e->type() != Expression::VARIABLE)
        return;

    // If we are the right hand side of an expression that is mul or pow,
    // default value should be 1
    double defaultValue = 0;
    Expression* p = e->parent();
    if (p != NULL && p->type() == Expression::FUNCTION2)
    {
        if (p->op2() == op::mul && e == p->right())
            defaultValue = 1;
        else if (p->op2() == op::pow && e == p->right())
            defaultValue = 1;
    }

    vt->add(e->name(), defaultValue);
}
VariableTable* Expression::generateVariableTable() const
{
    VariableTable* vt = new VariableTable;
    addVariableToTable(vt, this);
    return vt;
}

// ----------------------------------------------------------------------------
double Expression::evaluate(const VariableTable* vt) const
{
    switch (type())
    {
        case CONSTANT:  return value();
        case VARIABLE:  return vt->valueOf(name());
        case FUNCTION1: return op1()(
            right()->evaluate(vt)
        );
        case FUNCTION2: return op2()(
            left()->evaluate(vt),
            right()->evaluate(vt)
        );
        default: throw std::runtime_error("Error during evaluating expression: Found node marked with INVALID. Can't evaluate");
    }
}

// ----------------------------------------------------------------------------
bool Expression::isOperation(op::Op1 op) const
{
    return (type() == FUNCTION1 && op1() == op);
}

// ----------------------------------------------------------------------------
bool Expression::isOperation(op::Op2 op) const
{
    return (type() == FUNCTION2 && op2() == op);
}

// ----------------------------------------------------------------------------
bool Expression::hasRHSOperation(op::Op1 op) const
{
    return (right() && right()->type() == FUNCTION1 && right()->op1() == op);
}

// ----------------------------------------------------------------------------
bool Expression::hasRHSOperation(op::Op2 op) const
{
    return (right() && right()->type() == FUNCTION2 && right()->op2() == op);
}

// ----------------------------------------------------------------------------
bool Expression::hasVariable(const char* variable) const
{
    return (type() == VARIABLE && strcmp(name(), variable) == 0);
}
