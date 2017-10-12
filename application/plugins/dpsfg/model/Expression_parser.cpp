#include "model/Expression.hpp"
#include <cstring>

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
    if (*token_ >= 65 && *token_ <= 90)  // A-Z
        return true;
    if (*token_ >= 97 && *token_ <= 122)  // a-z
        return true;
    if (*token_ == '_')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isNumberToken()
{
    if (*token_ >= 48 && *token_ <= 57)
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool Expression::Parser::isOperatorToken()
{
    if (strchr("+-*/^", *token_))
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
Expression::Parser::Result Expression::Parser::makeSuccess(Expression* e)
{
    Expression* topMost = e;
    while (topMost->parent_ != NULL)
        topMost = topMost->parent_;

    Parser::Result result;
    result.expression_ = topMost;
    return result;
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::makeError(const char* msg)
{
    Parser::Result result;
    result.errorMessage_ = msg;
    result.errorColumn_ = token_ - str_;
    return result;
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::parse(const char* str)
{
    str_ = str;
    token_ = str;
    scope_ = 0;

    Expression* root = new Expression;
    Result result = expectOperandOrEnd(root);

    // Top-most expression is not refcounted anywhere and needs to be deleted
    if (result.isSuccess() == false)
    {
        while (root->parent_ != NULL)
            root = root->parent_;
        delete root;
    }
    return result;
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectOperand(Expression* e)
{
    advanceOverWhitespace();

    if (isSymbolToken())
        return expectSymbolName(e);
    if (isNumberToken())
        return expectNumber(e);
    if (isOpenBracket())
        return openScope(e);

    return makeError("Expected operand, got ");
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectOperandOrEnd(Expression* e)
{
    advanceOverWhitespace();
    if (isAtEnd())
        return makeSuccess(e);
    return expectOperand(e);
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectOperator(Expression* e)
{
    advanceOverWhitespace();
    if (isOperatorToken() == false)
        return makeError("Expected operator token, got ");

    e = e->asLHSOfSelf();
    e->operator_ = *token_;
    e = e->newRHS();
    advance();

    return expectOperand(e);
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectOperatorOrEnd(Expression* e)
{
    advanceOverWhitespace();
    if (isAtEnd())
        return makeSuccess(e);
    return expectOperator(e);
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectSymbolName(Expression* e)
{
    advanceOverWhitespace();
    if (isSymbolToken() == false)
        return makeError("Expected symbol token, got ");

    do
    {
        e->value_.append(*token_);
        advance();
    } while (isSymbolToken());

    return expectOperatorOrEnd(e);
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::expectNumber(Expression* e)
{
    advanceOverWhitespace();
    if (isNumberToken() == false)
        return makeError("Expected number token, got ");

    do
    {
        e->value_.append(*token_);
        advance();
    } while (isNumberToken());

    return expectOperator(e);
}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::openScope(Expression* e)
{

}

// ----------------------------------------------------------------------------
Expression::Parser::Result Expression::Parser::closeScope(Expression* e)
{

}

// ----------------------------------------------------------------------------
Expression* Expression::parse(const char* str)
{
    Parser parser;
    Parser::Result result = parser.parse(str);
    return result.expression_;
}
