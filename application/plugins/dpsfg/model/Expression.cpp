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
    if (type_ == VARIABLE)
        free(name_);
}

// ----------------------------------------------------------------------------
Expression* Expression::make(const char* symbolName)
{
    std::size_t len = strlen(symbolName);
    char* name = (char*)malloc((sizeof(char) + 1) * len);
    strcpy(name, symbolName);

    Expression* e = new Expression;
    e->type_ = VARIABLE;
    e->name_ = name;
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double value)
{
    Expression* e = new Expression;
    e->type_ = CONSTANT;
    e->value_ = value;
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double (*func1)(double), Expression* rhs)
{
    Expression* e = new Expression;
    e->type_ = FUNCTION1;
    e->eval1_ = func1;
    e->right_ = rhs;
    rhs->parent_ = e;

    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double (*func2)(double,double), Expression* lhs, Expression* rhs)
{
    Expression* e = new Expression;
    e->type_ = FUNCTION2;
    e->eval2_ = func2;
    e->left_ = lhs;
    e->right_ = rhs;
    lhs->parent_= e;
    rhs->parent_ = e;

    return e;
}

// ----------------------------------------------------------------------------
void Expression::addVariableToTable(VariableTable* vt, const Expression* e)
{
    if (e == NULL)
        return;

    addVariableToTable(vt, e->left());
    addVariableToTable(vt, e->right());

    if (e->type() != VARIABLE)
        return;

    // If we are the right hand side of an expression that is mul or pow,
    // default value should be 1
    double defaultValue = 0;
    Expression* p = e->parent();
    if (p != NULL && p->type() == FUNCTION2)
    {
        if (p->eval2() == op::mul && e == p->right())
            defaultValue = 1;
        else if (p->eval2() == op::pow && e == p->right())
            defaultValue = 1;
    }

    vt->add(e->name(), defaultValue);
}

// ----------------------------------------------------------------------------
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
        case FUNCTION1: return eval1()(
            right()->evaluate(vt)
        );
        case FUNCTION2: return eval2()(
            left()->evaluate(vt),
            right()->evaluate(vt)
        );
        default: throw std::runtime_error("Error during evaluating expression: Found node marked with INVALID. Can't evaluate");
    }
}
