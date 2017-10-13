#include "model/Expression.hpp"
#include <cstring>
#include <cctype>
#include <cmath>

using namespace dpsfg;

// ----------------------------------------------------------------------------
bool Expression::Parser::isAtEnd()
{
    if (*next_ == '\0')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isSymbol()
{
    if (isalpha(*next_))
        return true;
    if (*next_ == '_')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isNumber()
{
    return isdigit(*next_) || *next_ == '.';
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isOperator()
{
    if (strchr("+-*/^", *next_))
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isWhitespace()
{
    return std::isspace(*next_);
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isOpenBracket()
{
    return *next_ == '(';
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isCloseBracket()
{
    return *next_ == ')';
}

// ----------------------------------------------------------------------------
void Expression::Parser::advance()
{
    if (isAtEnd())
        return;
    ++next_;
}

// ----------------------------------------------------------------------------
void Expression::Parser::advanceOverWhitespace()
{
    while (isAtEnd() == false)
    {
        if (isWhitespace() == false)
            break;
        ++next_;
    }
}

// ----------------------------------------------------------------------------
void Expression::Parser::nextToken()
{
    type_ = TOK_NULL;

    do
    {
        if (*next_ == '\0')
        {
            type_ = TOK_END;
            return;
        }

        if (isNumber())
        {
            type_ = TOK_NUMBER;
            value_ = strtod(next_, (char**)&next_);
            continue;
        }

        if (isSymbol())
        {
            type_ = TOK_SYMBOL;
            name_.clear();
            do
                name_ += *next_++;
            while (isSymbol() || isdigit(*next_));
            continue;
        }

        switch (*next_++)
        {
            case '+': type_ = TOK_INFIX; function_ = &Symbol::add; break;
            case '-': type_ = TOK_INFIX; function_ = &Symbol::sub; break;
            case '*': type_ = TOK_INFIX; function_ = &Symbol::mul; break;
            case '/': type_ = TOK_INFIX; function_ = &Symbol::div; break;
            case '^': type_ = TOK_INFIX; function_ = &Symbol::pow; break;
            case '(': type_ = TOK_OPEN; break;
            case ')': type_ = TOK_CLOSE; break;
            case ' ': case '\t': case '\n': case '\r': continue;
            default: type_ = TOK_ERROR; break;
        }
    } while (type_ == TOK_NULL);
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::makeError(const char* msg)
{
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(const char* symbolName)
{
    int len = strlen(symbolName);
    char* name = (char*)malloc((sizeof(char) + 1) * len);
    strcpy(name, symbolName);

    Expression* e = new Expression;
    e->symbol_.type = Symbol::VARIABLE;
    e->symbol_.name = name;
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double value)
{
    Expression* e = new Expression;
    e->symbol_.type = Symbol::CONSTANT;
    e->symbol_.value = value;
    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double (Symbol::*func1)(double), Expression* rhs)
{
    Expression* e = new Expression;
    e->symbol_.type = Symbol::FUNCTION1;
    e->symbol_.eval1 = func1;
    e->right_ = rhs;
    rhs->parent_ = e;

    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::make(double (Symbol::*func2)(double,double), Expression* lhs, Expression* rhs)
{
    Expression* e = new Expression;
    e->symbol_.type = Symbol::FUNCTION2;
    e->symbol_.eval2 = func2;
    e->left_ = lhs;
    e->right_ = rhs;
    lhs->parent_= e;
    rhs->parent_ = e;

    return e;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::base()
{
    switch (type_)
    {
        case TOK_NUMBER:
        {
            double value = value_;
            nextToken();
            return Expression::make(value);
        }

        case TOK_SYMBOL:
        {
            std::string name = name_;
            nextToken();
            return Expression::make(name.c_str());
        }

        case TOK_OPEN:
        {
            nextToken();
            Expression* ret = expr();
            if (type_ != TOK_CLOSE)
                type_ = TOK_ERROR;
            else
                nextToken();
            return ret;
        }

        default:
            return new Expression;
    }
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::power()
{
    int sign = 1;
    while (type_ == TOK_INFIX && (function_ == &Symbol::add || function_ == &Symbol::sub))
    {
        if (function_ == &Symbol::sub)
            sign = -sign;
        nextToken();
    }

    if (sign == 1)
        return base();
    else
        return Expression::make(&Symbol::negate, base());
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::factor()
{
    Expression* ret = power();

    while (type_ == TOK_INFIX && (function_ == &Symbol::pow))
    {
        double (Symbol::*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, power());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::term()
{
    Expression* ret = factor();

    while (type_ == TOK_INFIX && (function_ == &Symbol::mul || function_ == &Symbol::div))
    {
        double (Symbol::*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, factor());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expr()
{
    Expression* ret = term();

    while (type_ == TOK_INFIX && (function_ == &Symbol::add || function_ == &Symbol::sub))
    {
        double (Symbol::*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, term());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::parse(const char* str)
{
    start_ = next_ = str;
    nextToken();

    Expression* result = expr();
    if (type_ != TOK_END)
    {
        delete result;
        return NULL;
    }

    return result;
}

// ----------------------------------------------------------------------------
Expression* Expression::parse(const char* str)
{
    Parser parser;
    return parser.parse(str);
}
