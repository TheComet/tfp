#pragma once

#include "tfp/util/Reference.hpp"
#include <string>
#include <unordered_map>
#include <cmath>

namespace dpsfg {

struct Symbol
{
    enum Type
    {
        INVALID,
        CONSTANT,
        VARIABLE,
        FUNCTION1,
        FUNCTION2
    };

    Symbol() : type(INVALID) {}

    double add(double a, double b) { return a + b; }
    double sub(double a, double b) { return a - b; }
    double mul(double a, double b) { return a * b; }
    double div(double a, double b) { return a / b; }
    double pow(double a, double b) { return std::pow(a, b); }
    double negate(double a) { return -a; }

    union {
        double value;
        char* name;
        double (Symbol::*eval1)(double);
        double (Symbol::*eval2)(double,double);
    };
    Type type;
};

class SymbolTable : public tfp::RefCounted
{
public:
    void add(std::string name, double value);
    void set(std::string name, double value);
    void remove(std::string name);
    void clear();
    double valueOf(std::string name, double=0, double=0);

private:
    std::unordered_map<std::string, Symbol> table_;
};

class Expression : public tfp::RefCounted
{
    struct Parser
    {
        enum Token
        {
            TOK_NULL,
            TOK_ERROR,
            TOK_END,
            TOK_OPEN,
            TOK_CLOSE,
            TOK_NUMBER,
            TOK_SYMBOL,
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

        Token type_;
        double value_;
        std::string name_;
        typedef double (Symbol::*OpFunc)(double,double);
        OpFunc function_;

        const char* start_;
        const char* next_;
        tfp::Reference<SymbolTable> symbolTable_;
    };
public:
    Expression();

    static Expression* parse(const char* expression);

    static Expression* make(double (Symbol::*func2)(double,double), Expression* lhs, Expression* rhs);
    static Expression* make(double (Symbol::*func1)(double), Expression* rhs);
    static Expression* make(double value);
    static Expression* make(const char* symbolName);

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
    const Symbol& symbol() const { return symbol_; }

private:
    Expression* parent_;
    Symbol symbol_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    tfp::Reference<SymbolTable> symbolTable_;
};

}
