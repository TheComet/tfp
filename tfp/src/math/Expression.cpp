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
Expression* Expression::shallowClone()
{
    Expression* e = new Expression;
    e->set(this);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::clone(Expression* parent)
{
    Expression* e = shallowClone();
    
    if (left())  e->left_  = left()->clone(e);
    if (right()) e->right_ = right()->clone(e);
    e->parent_ = parent;

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
    if (rhs->left()) rhs->left()->parent_ = rhs;
    if (rhs->right()) rhs->right()->parent_ = rhs;
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
    if (lhs->left()) lhs->left()->parent_ = lhs;
    if (lhs->right()) lhs->right()->parent_ = lhs;
    if (rhs->left()) rhs->left()->parent_ = rhs;
    if (rhs->right()) rhs->right()->parent_ = rhs;
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
    if (left_) left_->parent_ = this;
    if (right_) right_->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::reset()
{
    if (type_ == VARIABLE)
        free(name_);
    type_ = INVALID;
}

// ----------------------------------------------------------------------------
Expression* Expression::find(const char* variableName)
{
    if (type() == VARIABLE && strcmp(name(), variableName) == 0)
        return this;
    
    if (left())  return left()->find(variableName);
    if (right()) return right()->find(variableName);
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::find(double value)
{
    if (type() == CONSTANT && this->value() == value)
        return this;
    
    if (left())  return left()->find(value);
    if (right()) return right()->find(value);
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::find(op::Op1 func)
{
    if (type() == FUNCTION1 && op1() == func)
        return this;
    
    if (left())  return left()->find(func);
    if (right()) return right()->find(func);
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::find(op::Op2 func)
{
    if (type() == FUNCTION2 && op2() == func)
        return this;
    
    if (left())  return left()->find(func);
    if (right()) return right()->find(func);
    return NULL;
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
bool Expression::isSameAs(Expression* other) const
{
    if (type() != other->type())
        return false;

    switch (type())
    {
        case CONSTANT  : return value() == other->value();
        case VARIABLE  : return strcmp(name(), other->name()) == 0;
        case FUNCTION1 : return op1() == other->op1();
        case FUNCTION2 : return op2() == other->op2();
        default        : return false;
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
Expression* Expression::getOtherOperand() const
{
    assert(type() == FUNCTION2);
    return parent()->left() == this ? parent()->right() : parent()->left();
}

// ----------------------------------------------------------------------------
bool Expression::recursivelyCall(bool (Expression::*optfunc)())
{
    bool mutated = false;
    if (left())  mutated |= left()->recursivelyCall(optfunc);
    if (right()) mutated |= right()->recursivelyCall(optfunc);
    mutated |= (this->*optfunc)();
    return mutated;
}

// ----------------------------------------------------------------------------
bool Expression::recursivelyCall(bool (Expression::*optfunc)(const char*), const char* variable)
{
    bool mutated = false;
    if (left())  mutated |= left()->recursivelyCall(optfunc, variable);
    if (right()) mutated |= right()->recursivelyCall(optfunc, variable);
    mutated |= (this->*optfunc)(variable);
    return mutated;
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
int guid = 1;
static void dumpRecurse(FILE* fp, Expression* e)
{
    std::size_t thisId = (std::size_t)e;
    fprintf(fp, "    %lu [label=", thisId);
    writeName(fp, e);
    fprintf(fp, "];\n");

    if (e->right())
    {
        dumpRecurse(fp, e->right());
        fprintf(fp, "    %lu -> %lu [label=rhs];\n", thisId, (std::size_t)e->right());
    }
    if (e->left())
    {
        dumpRecurse(fp, e->left());
        fprintf(fp, "    %lu -> %lu [label=lhs];\n", thisId, (std::size_t)e->left());
    }
    if (e->parent())
    {
        fprintf(fp, "    %lu -> %lu [color=\"0.2 1.0 1.0\"];\n", thisId, (std::size_t)e->parent());
    }
}
void Expression::dump(FILE* fp)
{
    fprintf(fp, "digraph graphname {\n");
    fprintf(fp, "    %lu [color=\"0.0 1.0 1.0\"];\n", (std::size_t)this);
    dumpRecurse(fp, this);
    fprintf(fp, "}\n\n");
}
void Expression::dump(const char* filename, bool append)
{
    const char* mode = append ? "a" : "w";
    FILE* fp = fopen(filename, mode);
    dump(fp);
    fclose(fp);
}
void Expression::dump()
{
    if (fp)
        dump(fp);
}
