#include "tfp/math/new/Expression.hpp"
#include "tfp/math/new/ExpressionParser.hpp"
#include <iostream>

void yyerror(const char* m)
{
}

namespace tfpnew {
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

const char* Expression::yyString_;
int Expression::yyOffset_;

// ----------------------------------------------------------------------------
Expression::Expression() :
    parent_(nullptr),
    type_(INVALID)
{
}

// ----------------------------------------------------------------------------
Expression::~Expression()
{
    makeInvalid();
}

// ----------------------------------------------------------------------------
Expression* Expression::parse(const char* str)
{
    yyOffset_ = 0;
    yyString_ = str;
    ::yyparse();
    
    /*
    ExpressionParser parser;
    Expression* e = parser.parse(str);
    if (e == NULL)
        return NULL;

    e->optimise();
    return e;*/
    return nullptr;
}

// ----------------------------------------------------------------------------
int Expression::yyInput(char* buf, int buflen)
{
    int i;
    for (i = 0; i < buflen; ++i)
    {
        buf[i] = yyString_[yyOffset_ + i];
        if (!buf[i])
            break;
    }
    yyOffset_ += i;
    return i;
}

// ----------------------------------------------------------------------------
Expression* Expression::makeInvalid()
{
    return new Expression();
}

// ----------------------------------------------------------------------------
Expression* Expression::make(const char* variableName)
{
    Expression* e = Expression::makeInvalid();
    e->set(variableName);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double value)
{
    Expression* e = Expression::makeInvalid();
    e->set(value);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(op::Op1 func, Expression* rhs)
{
    Expression* e = Expression::makeInvalid();
    e->set(func, rhs);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(op::Op2 func, Expression* lhs, Expression* rhs)
{
    Expression* e = Expression::makeInvalid();
    e->set(func, lhs, rhs);
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::makeInfinity()
{
    Expression* e = Expression::makeInvalid();
    e->setInfinity();
    return e;
}

// ----------------------------------------------------------------------------
void Expression::set(const char* variableName)
{
    setInvalid();

    type_  = VARIABLE;
    left_  = NULL;
    right_ = NULL;
    data_.name_ = (char*)malloc((strlen(variableName) + 1) * sizeof(char));
    strcpy(data_.name_, variableName);
}

// ----------------------------------------------------------------------------
void Expression::set(double value)
{
    setInvalid();

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
        right_ = Expression::makeInvalid();
        right_->stealDataFrom(this);
    }
    else
    {
        makeInvalid();
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
        Expression* e = Expression::makeInvalid();
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
        setInvalid();
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
    setInvalid();

    type_ = INF;
    left_ = NULL;
    right_ = NULL;
}

// ----------------------------------------------------------------------------
void Expression::setInvalid()
{
    if (type_ == VARIABLE)
        free(data_.name_);
    type_ = INVALID;
}

// ----------------------------------------------------------------------------
Expression* Expression::clone(Expression* parent) const
{
    Expression* e = Expression::makeInvalid();
    e->copyDataFrom(this);
    e->parent_ = parent;

    if (left())  e->left_  = left()->clone(e);
    if (right()) e->right_ = right()->clone(e);

    return e;
}

// ----------------------------------------------------------------------------
void Expression::copyDataFrom(const Expression* other)
{
    if (other->type() == VARIABLE)
    {
        char* newVariable = (char*)malloc((strlen(other->name()) + 1) * sizeof(char));
        if (newVariable == nullptr)
            throw std::bad_alloc();
        strcpy(newVariable, other->name());
        
        setInvalid();
        data_.name_ = newVariable;
        type_ = other->type_;
    }
    else
    {
        data_ = other->data_;
        type_ = other->type_;
    }
}

// ----------------------------------------------------------------------------
void Expression::stealDataFrom(Expression* other)
{
    setInvalid();

    type_ = other->type_;
    data_ = other->data_;

    other->type_ = INVALID;
}

// ----------------------------------------------------------------------------
int Expression::countNodes() const
{
    int count = 1;
    if (left()) count += left()->countNodes();
    if (right()) count += right()->countNodes();
    return count;
}

// ----------------------------------------------------------------------------
#ifdef DEBUG
bool Expression::checkConsistencies() const
{
    bool success = true;
    if (left())  success &= left()->checkConsistencies();
    if (right()) success &= right()->checkConsistencies();

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
#endif

}
