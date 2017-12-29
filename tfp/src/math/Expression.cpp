#include "tfp/math/Expression.hpp"
#include "tfp/math/ExpressionParser.hpp"
#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Util.hpp"
#include "tfp/util/Tears.hpp"
#include <string.h>
#include <cassert>
#include <cmath>
#include <iostream>

namespace tfp {
namespace op {

double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mul(double a, double b) { return a * b; }
double div(double a, double b) { return a / b; }
double pow(double a, double b) { return std::pow(a, b); }
double mod(double a, double b) { return std::fmod(a, b); }
double negate(double a) { return -a; }
double comma(double a, double b) { return b; }

}
}

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
Expression* Expression::parse(const char* str)
{
    ExpressionParser parser;
    Expression* e = parser.parse(str);
    if (e == NULL)
        return NULL;

    e->optimise();
    return e;
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
Expression* Expression::makeInfinity()
{
    Expression* e = new Expression;
    e->setInfinity();
    return e;
}

// ----------------------------------------------------------------------------
void Expression::copyDataFrom(const Expression* other)
{
    type_ = other->type();

    if (other->type() == VARIABLE)
    {
        data_.name_ = (char*)malloc((strlen(other->name()) + 1) * sizeof(char));
        strcpy(data_.name_, other->name());
    }
    else
    {
        memcpy(&data_, &other->data_, sizeof(data_));
    }
}

// ----------------------------------------------------------------------------
void Expression::stealDataFrom(Expression* other)
{
    type_ = other->type_;
    data_ = other->data_;

    other->type_ = INVALID;
}

// ----------------------------------------------------------------------------
Expression* Expression::unlinkExchange(Expression* other)
{
    // Parent's reference to us
    Reference<Expression>& operand = parent()->left() == this ? parent()->left_ : parent()->right_;
    operand.detach();
    operand = other; // Replaces the parent's reference to us with "other"
    other->parent_ = parent(); // Relink other's parent correctly
    parent_ = NULL; // Since parent doesn't reference us anymore, we should do the same
    return this;
}

// ----------------------------------------------------------------------------
void Expression::replaceWith(Expression* other)
{
    Reference<Expression> ref(other);
    stealDataFrom(other);
    left_ = other->left_;
    right_ = other->right_;
    if (left_)  left_->parent_ = this;
    if (right_) right_->parent_ = this;

}

// ----------------------------------------------------------------------------
Expression* Expression::unlinkFromTree()
{
    if (parent() == NULL)
        return this;

    Reference<Expression>& operand = parent()->left() == this ? parent()->left_ : parent()->right_;
    operand.detach();
    parent_ = NULL;

    return this;
}

// ----------------------------------------------------------------------------
Expression* Expression::clone(Expression* parent) const
{
    Expression* e = new Expression;
    e->copyDataFrom(this);
    e->parent_ = parent;

    if (left())  e->left_  = left()->clone(e);
    if (right()) e->right_ = right()->clone(e);

    return e;
}

// ----------------------------------------------------------------------------
void Expression::swapOperands()
{
    assert(type() == FUNCTION2);
    Reference<Expression> tmp = right_;
    right_ = left_;
    left_ = tmp;
}

// ----------------------------------------------------------------------------
void Expression::collapseIntoParent()
{
    Expression* p = parent();
    if (p == NULL)
        return;

    p->stealDataFrom(this);

    addRef();
    p->left_  = left();
    p->right_ = right();
    parent_ = NULL;
    releaseRef();

    if (p->left_)  p->left_->parent_ = p;
    if (p->right_) p->right_->parent_ = p;
}

// ----------------------------------------------------------------------------
void Expression::set(const char* variableName)
{
    reset();

    type_  = VARIABLE;
    left_  = NULL;
    right_ = NULL;
    data_.name_ = (char*)malloc((sizeof(char) + 1) * strlen(variableName));
    strcpy(data_.name_, variableName);
}

// ----------------------------------------------------------------------------
void Expression::set(double value)
{
    reset();

    type_  = CONSTANT;
    left_  = NULL;
    right_ = NULL;
    data_.value_ = value;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op1 func, Expression* rhs)
{
    if (rhs == this)
    {
        right_ = new Expression;
        right_->stealDataFrom(this);
    }
    else
    {
        reset();
        right_ = rhs;
    }

    type_        = FUNCTION1;
    data_.op1_   = func;
    left_        = NULL;
    rhs->parent_ = this;
}

// ----------------------------------------------------------------------------
void Expression::set(op::Op2 func, Expression* lhs, Expression* rhs)
{
    assert(rhs != this || lhs != this);

    if (rhs == this || lhs == this)
    {
        // We need to become our own LHS/RHS, so first, copy ourselves into a
        // new expression node
        Expression* e = new Expression;
        e->stealDataFrom(this);
        e->left_ = left_;
        e->right_ = right_;
        if (e->left_)  e->left_->parent_ = e;
        if (e->right_) e->right_->parent_ = e;

        if (rhs == this)
        {
            // Now assign new node as RHS
            right_ = e;
            left_ = lhs;
        }
        else
        {
            // Now assign new node as LHS
            left_ = e;
            right_ = rhs;
        }

        left_->parent_ = this;
        right_->parent_ = this;
    }
    else
    {
        reset();
        rhs->addRef();  // our left() might be the current rhs object
        left_ = lhs;
        right_ = rhs;
        rhs->releaseRef();
        left_->parent_ = this;
        right_->parent_ = this;
    }

    type_ = FUNCTION2;
    data_.op2_ = func;

    if (lhs->left()) lhs->left()->parent_ = lhs;
    if (lhs->right()) lhs->right()->parent_ = lhs;
    if (rhs->left()) rhs->left()->parent_ = rhs;
    if (rhs->right()) rhs->right()->parent_ = rhs;
}

// ----------------------------------------------------------------------------
void Expression::setInfinity()
{
    reset();

    type_ = INF;
    left_ = NULL;
    right_ = NULL;
}

// ----------------------------------------------------------------------------
void Expression::reset()
{
    if (type_ == VARIABLE)
        free(data_.name_);
    type_ = INVALID;
}

// ----------------------------------------------------------------------------
bool Expression::optimise()
{
    ExpressionOptimiser optimise;
    return optimise.everything(this);
}

// ----------------------------------------------------------------------------
Expression* Expression::find(const char* variableName)
{
    return find(MatchVariable(variableName));
}

// ----------------------------------------------------------------------------
Expression* Expression::find(double value)
{
    return find(MatchValue(value));
}

// ----------------------------------------------------------------------------
Expression* Expression::find(op::Op1 func)
{
    return find(MatchOp1(func));
}

// ----------------------------------------------------------------------------
Expression* Expression::find(op::Op2 func)
{
    return find(MatchOp2(func));
}

// ----------------------------------------------------------------------------
Expression* Expression::findSame(const Expression* match)
{
    return find(MatchSame(match), RecurseAll(), this);
}

// ----------------------------------------------------------------------------
Expression* Expression::findOpWithSameLHS(op::Op2 func, const Expression* match)
{
    if (parent() && this != match && this != parent()->right() &&
            parent()->isOperation(func) && isSameAs(match))
        return parent();

    Expression* e;
    if (left())  if ((e = left()->findOpWithSameLHS(func, match)) != NULL) return e;
    if (right()) if ((e = right()->findOpWithSameLHS(func, match)) != NULL) return e;
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::findOpWithNegativeRHS(op::Op2 func)
{
    if (isOperation(func) && right()->type() == CONSTANT && right()->value() < 0.0)
        return this;

    Expression* e;
    if (left())  if ((e = left()->findOpWithNegativeRHS(func)) != NULL) return e;
    if (right()) if ((e = right()->findOpWithNegativeRHS(func)) != NULL) return e;
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::travelUpChain(op::Op2 func)
{
    Expression* top = this;
    while (top->parent() != NULL && top->parent()->isOperation(func))
        top = top->parent();
    return top;
}

// ----------------------------------------------------------------------------
Expression* Expression::findSameDownChain(op::Op2 func, const Expression* match)
{
    if (this != match && isSameAs(match))
        return this;

    if (isOperation(func) == false)
        return NULL;

    Expression* e;
    if ((e = left()->findSameDownChain(func, match)) != NULL) return e;
    if ((e = right()->findSameDownChain(func, match)) != NULL) return e;
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::travelUpComplementaryChain(op::Op2 func1, op::Op2 func2, double* negated)
{
    Expression* top = this;
    *negated = 1.0;

    while (top->parent() != NULL)
    {
        if (top->parent()->isOperation(func1))
        {
            top = top->parent();
            continue;
        }
        if (top->parent()->isOperation(func2))
        {
            top = top->parent();
            *negated = -*negated;
            continue;
        }

        break;  // No more parent operation found
    }

    return top;
}

// ----------------------------------------------------------------------------
Expression* Expression::findSameDownComplementaryChain(op::Op2 func1, op::Op2 func2, double* negated, Expression* match)
{
    *negated = 1.0;
    if (this != match && isSameAs(match))
        return this;

    if (isOperation(func1) == false && isOperation(func2) == false)
        return NULL;

    if (isOperation(func2))
        *negated = -*negated;

    Expression* e;
    double childNegated;
    if ((e = left()->findSameDownComplementaryChain(func1, func2, &childNegated, match)) != NULL)
    {
        *negated = *negated * childNegated;
        return e;
    }
    if ((e = right()->findSameDownComplementaryChain(func1, func2, &childNegated, match)) != NULL)
    {
        *negated = *negated * childNegated;
        return e;
    }
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

    vt->set(e->name(), defaultValue);
}
VariableTable* Expression::generateVariableTable() const
{
    VariableTable* vt = new VariableTable;
    addVariableToTable(vt, this);
    return vt;
}

// ----------------------------------------------------------------------------
static bool insertSubstitutionsRecursive(const VariableTable* vt,
                                         Expression* e,
                                         std::set<std::size_t>* visited)
{
    if (e->left()) insertSubstitutionsRecursive(vt, e->left(), visited);
    if (e->right()) insertSubstitutionsRecursive(vt, e->right(), visited);

    /*
     * A resolved variable could lead to another variable that also exists in
     * the variable table, so keep resolving into no more are found.
     */
    Expression* resolved;
    while (e->type() == Expression::VARIABLE &&
            (resolved = vt->get(e->name())) != NULL)
    {
        /*
         * Keep track of which expression node was substituted with which
         * resolved expression so we can detect cyclic substitutions. The
         * easiest way to do this is to generate a "combined hash" from the
         * expression and the resolved variable, and dump it into a set. If in
         * the future the hash already exists in the set, then we know we're
         * about to substitute the same variable a second time.
         */
        std::size_t hash = Util::combineHashes(std::size_t(e), std::size_t(resolved));
        if (visited->insert(hash).second == false)
            return false;

        // Clone resolved expression and replace variable with it
        e->replaceWith(resolved->clone());
    }

    return true;
}
bool Expression::insertSubstitutions(const VariableTable* vt)
{
    std::set<std::size_t> visited;
    return insertSubstitutionsRecursive(vt, this, &visited);
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
        default:
            g_tears.cry("Error during evaluating expression: Found node marked with INVALID. Can't evaluate");
            return std::numeric_limits<double>::quiet_NaN();
    }
}

// ----------------------------------------------------------------------------
bool Expression::isSameAs(const Expression* other) const
{
    if (type() != other->type())
        return false;
    if (left() && other->left() && left()->isSameAs(other->left()) == false)
        return false;
    if (right() && other->right() && right()->isSameAs(other->right()) == false)
        return false;

    switch (type())
    {
        case CONSTANT  : return (value() == other->value());
        case VARIABLE  : return (strcmp(name(), other->name()) == 0);
        case FUNCTION1 : return (op1() == other->op1());
        case FUNCTION2 : return (op2() == other->op2());
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
int Expression::size() const
{
    int count = 1;
    if (left()) count += left()->size();
    if (right()) count += right()->size();
    return count;
}

// ----------------------------------------------------------------------------
Expression* Expression::getOtherOperand() const
{
    assert(parent() != NULL);
    assert(parent()->type() == FUNCTION2);
    return parent()->left() == this ? parent()->right() : parent()->left();
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
    if (f == op::add) fprintf(fp, "+");
    else if (f == op::sub) fprintf(fp, "-");
    else if (f == op::mul) fprintf(fp, "*");
    else if (f == op::div) fprintf(fp, "/");
    else if (f == op::pow) fprintf(fp, "^");
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
        case Expression::INF       : fprintf(fp, "inf"); break;
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
    fprintf(fp, "    %lu [label=\"", thisId);
    writeName(fp, e);
    fprintf(fp, "\"];\n");

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
void Expression::dump(FILE* fp, const char* context)
{
    fprintf(fp, "digraph graphname {\n");
    if (strlen(context) > 0)
        fprintf(fp, "    context [label=\"%s\"];\n", context);
    fprintf(fp, "    %lu [color=\"0.0 1.0 1.0\"];\n", (std::size_t)this);
    dumpRecurse(fp, this);
    fprintf(fp, "}\n\n");
}
void Expression::dump(const char* filename, bool append, const char* context)
{
    const char* mode = append ? "a" : "w";
    FILE* fp = fopen(filename, mode);
    dump(fp, context);
    fclose(fp);
}
void Expression::dump()
{
    if (fp)
        dump(fp);
}
