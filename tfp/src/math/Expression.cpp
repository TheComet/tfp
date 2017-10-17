#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include <string.h>
#include <cassert>

using namespace tfp;

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
Expression* Expression::clone()
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
    left_ = NULL;
    right_ = NULL;
    name_ = (char*)malloc((sizeof(char) + 1) * strlen(variableName));
    strcpy(name_, variableName);
}

// ----------------------------------------------------------------------------
void Expression::set(double value)
{
    reset();

    type_ = CONSTANT;
    left_ = NULL;
    right_ = NULL;
    value_ = value;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op1 func, Expression* rhs)
{
    reset();

    type_ = FUNCTION1;
    op1_ = func;
    right_ = rhs;
    left_ = NULL;
    rhs->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op2 func, Expression* lhs, Expression* rhs)
{
    reset();

    type_ = FUNCTION2;
    op2_ = func;
    tfp::Reference<Expression> ptrl(lhs);
    tfp::Reference<Expression> ptrr(rhs);
    left_ = lhs;
    right_ = rhs;
    lhs->parent_ = this;
    rhs->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::set(Expression* other)
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

    tfp::Reference<Expression> ptr(other);
    left_ = other->left_;
    right_ = other->right_;
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
double Expression::evaluate(const VariableTable* vt, std::set<std::string>* visited) const
{
    switch (type())
    {
        case CONSTANT:  return value();
        case VARIABLE:  return vt->valueOf(name(), visited);
        case FUNCTION1: return op1()(right()->evaluate(vt, visited));
        case FUNCTION2: return op2()(
            left()->evaluate(vt, visited),
            right()->evaluate(vt, visited)
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

// ----------------------------------------------------------------------------
FILE* fp = NULL;
void beginDump(const char* filename)
{
    fp = fopen(filename, "w");
}
void endDump()
{
    if (fp) fclose(fp);
    fp = NULL;
}
static void writeFunction(FILE* fp, op::Op2 f)
{
    if (f == op::add) fprintf(fp, "\"+\"");
    else if (f == op::sub) fprintf(fp, "\"-\"");
    else if (f == op::mul) fprintf(fp, "\"*\"");
    else if (f == op::div) fprintf(fp, "\"/\"");
    else if (f == op::pow) fprintf(fp, "\"^\"");
}
static void writeFunction(FILE* fp, op::Op1 f)
{
    if (f == op::negate) fprintf(fp, "neg");
}
void writeName(FILE* fp, Expression* e)
{
    switch (e->type())
    {
        case Expression::CONSTANT  : fprintf(fp, "%f", e->value()); break;
        case Expression::VARIABLE  : fprintf(fp, "%s", e->name());  break;
        case Expression::FUNCTION1 : writeFunction(fp, e->op1()); break;
        case Expression::FUNCTION2 : writeFunction(fp, e->op2()); break;
        case Expression::INVALID : break;
    }
}
int guid = 0;
static int dumpRecurse(FILE* fp, Expression* e)
{
    int thisId = guid++;
    fprintf(fp, "    %d [label=", thisId);
    writeName(fp, e);
    fprintf(fp, "];\n");

    if (e->right())
    {
        fprintf(fp, "    %d -- %d [label=rhs];\n", thisId, dumpRecurse(fp, e->right()));
    }
    if (e->left())
    {
        fprintf(fp, "    %d -- %d [label=lhs];\n", thisId, dumpRecurse(fp, e->left()));
    }

    return thisId;
}
void Expression::dump(FILE* fp)
{
    fprintf(fp, "graph graphname {\n");
    fprintf(fp, "    %d [color=\"0.0 1.0 1.0\"];\n", guid);
    dumpRecurse(fp, this);
    fprintf(fp, "}\n\n");
}
void Expression::dump(const char* filename)
{
    FILE* fp = fopen(filename, "w");
    dump(fp);
    fclose(fp);
}
void Expression::dump()
{
    if (fp)
        dump(fp);
}
