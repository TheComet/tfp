#pragma once

#include "tfp/util/Reference.hpp"
#include <QString>

namespace dpsfg {

class Expression : public tfp::RefCounted
{
    struct Parser
    {
        struct Result
        {
            Result() : expression_(NULL) {}

            bool isSuccess() { return expression_ != NULL; }

            QString errorMessage_;
            int errorColumn_;
            Expression* expression_;
        };

        bool isAtEnd();
        bool isSymbolToken();
        bool isNumberToken();
        bool isOperatorToken();
        bool isWhitespaceToken();
        bool isOpenBracket();
        bool isCloseBracket();

        void advance();
        void advanceOverWhitespace();

        Result makeSuccess(Expression* e);
        Result makeError(const char* msg);
        Result openScope(Expression* e);
        Result closeScope(Expression* e);
        Result expectOperand(Expression* e);
        Result expectOperandOrEnd(Expression* e);
        Result expectOperatorOrEnd(Expression* e);
        Result expectOperator(Expression* e);
        Result expectSymbolName(Expression* e);
        Result expectNumber(Expression* e);

        Result parse(const char* str);

        const char* str_;
        const char* token_;
        int scope_;
    };
public:
    Expression();

    static Expression* parse(const char* expression);

    Expression* asLHSOfSelf();
    Expression* newRHS();

    Expression* parent() const { return parent_; }
    Expression* left() const { return left_; }
    Expression* right() const { return right_; }
    const char* value() const { return value_.toLatin1().data(); }
    char op() const { return operator_; }


private:
    Expression* parent_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    QString value_;
    char operator_;
};

//inline Expression operator+(Expression lhs, const Expression& rhs) { lhs += rhs; return lhs;}
//inline Expression operator-(Expression lhs, const Expression& rhs) { lhs -= rhs; return lhs;}
//inline Expression operator*(Expression lhs, const Expression& rhs) { lhs *= rhs; return lhs;}
//inline Expression operator/(Expression lhs, const Expression& rhs) { lhs /= rhs; return lhs;}
//inline Expression operator^(Expression lhs, const Expression& rhs) { lhs ^= rhs; return lhs;}

}
