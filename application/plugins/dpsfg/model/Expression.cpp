#include "Expression.hpp"
#include <string.h>

using namespace dpsfg;

static const char* supportedOperations = "+-*/^";

// ----------------------------------------------------------------------------
Expression::Expression() :
    parent_(NULL),
    left_(NULL),
    right_(NULL),
    operation_('\0')
{
}

// ----------------------------------------------------------------------------
Expression::~Expression()
{
    if (left_ != NULL)
        delete left_;
    if (right_ != NULL)
        delete right_;
}

// ----------------------------------------------------------------------------
Expression& Expression::operator=(const char* expression)
{
    // Currently, we don't support arbitrary expressions, only assignments of symbol names or numbers
    if (strpbrk(expression, supportedOperations) != NULL)
        return *this;

    //children_.clear();
    symbolName_ = expression;
    operation_ = '\0';

    return *this;
}

// ----------------------------------------------------------------------------
//Expression& Expression::combine(Expression* rhs)
//{
//    Expression* lhs = new Expression;
//    lhs->left_       = left_;
//    lhs->right_      = right_;
//    lhs->symbolName_ = symbolName_;
//    lhs->operation_  = operation_;
//
//    left_  = lhs;  rhs->parent_ = this;
//    right_ = rhs;  lhs->parent_ = this;
//
//    operation_ = '+';
//    symbolName_ = "";
//
//    return *this;
//}
