#pragma once

#include "tfp/util/Reference.hpp"
#include <string>
#include <unordered_map>

namespace dpsfg {

class SymbolTable
{
public:
    void add(std::string name, double value);
    void set(std::string name, double value);
    void remove(std::string name);
    double evaluate(std::string name);

private:
    union Symbol
    {
        double (*func)(double,double);
        double value;
    };
    std::unordered_map<std::string, Symbol> table_;
};

class Expression : public tfp::RefCounted
{
    struct Parser
    {
        enum
        {
            TOK_ERROR,
            TOK_END,
            TOK_OPEN,
            TOK_CLOSE,
            TOK_NUMBER,
            TOK_VARIABLE,
            TOK_INFIX
        };

        bool isAtEnd();
        bool isSymbol();
        bool isNumber();
        bool isOperator();
        bool isWhitespace();
        bool isOpenBracket();
        bool isCloseBracket();

        void advance();
        void advanceOverWhitespace();
        void nextToken();

        Expression* makeError(const char* msg);
        Expression* power();
        Expression* base();
        Expression* factor();
        Expression* term();
        Expression* expr();

        Expression* parse(const char* str);

        const char* start_;
        const char* next_;
        SymbolTable symbols_;
    };
public:
    Expression();

    static Expression* parse(const char* expression);

private:
    void doRestOfInsert(Expression* inserted);
public:
    Expression* makeLHSOfSelf();
    Expression* makeRHSOfSelf();
    Expression* newLHS();
    Expression* newRHS();

public:

    Expression* root() {
        Expression* root = this;
        while (root->parent_ != NULL)
            root = root->parent_;
        return root;
    }
    Expression* parent() const { return parent_; }
    Expression* left() const { return left_; }
    Expression* right() const { return right_; }
    const char* value() const { return value_.c_str(); }
    char op() const { return operator_; }

private:
    Expression* parent_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    std::string value_;
    char operator_;
};

}
