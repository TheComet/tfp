#include "model/Expression.hpp"
#include <cstring>
#include <cctype>
#include <cmath>

namespace dpsfg {
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
            case '+': type_ = TOK_INFIX; function_ = op::add; break;
            case '-': type_ = TOK_INFIX; function_ = op::sub; break;
            case '*': type_ = TOK_INFIX; function_ = op::mul; break;
            case '/': type_ = TOK_INFIX; function_ = op::div; break;
            case '^': type_ = TOK_INFIX; function_ = op::pow; break;
            case '%': type_ = TOK_INFIX; function_ = op::mod; break;
            case '(': type_ = TOK_OPEN; break;
            case ')': type_ = TOK_CLOSE; break;
            case ',': type_ = TOK_SEP; break;
            case ' ': case '\t': case '\n': case '\r': continue;
            default: type_ = TOK_ERROR; break;
        }
    } while (type_ == TOK_NULL);
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
    while (type_ == TOK_INFIX && (function_ == op::add || function_ == op::sub))
    {
        if (function_ == op::sub)
            sign = -sign;
        nextToken();
    }

    if (sign == 1)
        return base();
    else
        return Expression::make(op::negate, base());
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::factor()
{
    Expression* ret = power();

    while (type_ == TOK_INFIX && (function_ == op::pow))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, power());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::term()
{
    Expression* ret = factor();

    while (type_ == TOK_INFIX && (function_ == op::mul || function_ == op::div || function_ == op::mod))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, factor());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expr()
{
    Expression* ret = term();

    while (type_ == TOK_INFIX && (function_ == op::add || function_ == op::sub))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, term());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::list()
{
    Expression* ret = expr();

    while (type_ == TOK_SEP)
    {
        nextToken();
        ret = Expression::make(op::comma, ret, expr());
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
    Expression* e = parser.parse(str);
    if (e == NULL)
        return NULL;

    e->optimise();
    return e;
}
