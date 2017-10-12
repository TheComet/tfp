#include "model/Expression.hpp"
#include <cstring>
#include <cctype>

using namespace dpsfg;

// ----------------------------------------------------------------------------
bool Expression::Parser::isAtEnd()
{
    if (*token_ == '\0')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isSymbolToken()
{
    if (isalpha(*next_))
        return true;
    if (*next_ == '_')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isNumberToken()
{
    return isdigit(*next_);
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isOperatorToken()
{
    if (strchr("+-*/^", *next_))
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isWhitespaceToken()
{
    return std::isspace(*token_);
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isOpenBracket()
{
    return *token_ == '(';
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isCloseBracket()
{
    return *token_ == ')';
}

// ----------------------------------------------------------------------------
void Expression::Parser::advance()
{
    if (isAtEnd())
        return;
    ++token_;
}

// ----------------------------------------------------------------------------
void Expression::Parser::advanceOverWhitespace()
{
    while (isAtEnd() == false)
    {
        if (isWhitespaceToken() == false)
            break;
        ++token_;
    }
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::makeError(const char* msg)
{
    return NULL;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectOperand()
{
    advanceOverWhitespace();

    if (isSymbolToken())
        return expectSymbolName();
    if (isNumberToken())
        return expectNumber();
    if (isOpenBracket())
        return openScope();

    return makeError("Expected operand, got ");
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectOperandOrEnd()
{
    advanceOverWhitespace();
    if (isAtEnd() || isCloseBracket())
        return NULL;

    return expectOperand();
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectOperator(Expression* lhs)
{
    advanceOverWhitespace();
    if (isOperatorToken() == false)
        return makeError("Expected operator token, got ");

    Expression* op = new Expression;
    op->operator_ = *token_;
    advance();
    op->left_ = lhs;
    op->right_ = expectOperand();

    return op;
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectOperatorOrEnd(Expression* e)
{
    advanceOverWhitespace();
    if (isAtEnd() || isCloseBracket())
        return NULL;

    return expectOperator(e);
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectSymbolName()
{
    advanceOverWhitespace();
    if (isSymbolToken() == false)
        return makeError("Expected symbol token, got ");

    Expression* e = new Expression;
    do
    {
        e->value_ += *token_;
        advance();
    } while (isSymbolToken());

    return expectOperatorOrEnd(e);
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::expectNumber()
{
    advanceOverWhitespace();
    if (isNumberToken() == false)
        return makeError("Expected number token, got ");

    Expression* e = new Expression;
    do
    {
        e->value_ += *token_;
        advance();
    } while (isNumberToken());

    return expectOperator(e);
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::openScope()
{
    advance();
    return expectOperand();
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::closeScope()
{
    return makeError("oh oh");
}

// ----------------------------------------------------------------------------
Expression* Expression::Parser::parse(const char* str)
{
    str_ = str;
    token_ = str;
    scope_ = 0;

    return expectOperandOrEnd();
}

// ----------------------------------------------------------------------------
Expression* Expression::parse(const char* str)
{
    Parser parser;
    return parser.parse(str);
}
