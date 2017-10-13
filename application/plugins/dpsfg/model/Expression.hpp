#pragma once

#include "tfp/util/Reference.hpp"
#include <string>
#include <unordered_map>

namespace dpsfg {

namespace op {

double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
double div(double a, double b);
double pow(double a, double b);
double mod(double a, double b);
double negate(double a);
double comma(double a, double b);

}

class VariableTable;

class Expression : public tfp::RefCounted
{
    struct Parser
    {
        enum Token
        {
            TOK_NULL,
            TOK_ERROR,
            TOK_END,
            TOK_SEP,
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

        void nextToken();

        Expression* base();
        Expression* power();
        Expression* factor();
        Expression* term();
        Expression* expr();
        Expression* list();

        Expression* parse(const char* str);

        Token type_;
        double value_;
        std::string name_;
        double (*function_)(double,double);

        const char* start_;
        const char* next_;
    };

public:
    enum Type
    {
        INVALID,
        CONSTANT,
        VARIABLE,
        FUNCTION1,
        FUNCTION2
    };

    Expression();
    ~Expression();

    static Expression* parse(const char* expression);

    static Expression* make(double (*func2)(double,double), Expression* lhs, Expression* rhs);
    static Expression* make(double (*func1)(double), Expression* rhs);
    static Expression* make(double value);
    static Expression* make(const char* symbolName);

private:
    static void addVariableToTable(VariableTable* vt, const Expression* e);
public:
    VariableTable* generateVariableTable() const;
    double evaluate(const VariableTable* vt=NULL) const;

    Expression* root() {
        Expression* root = this;
        while (root->parent_ != NULL)
            root = root->parent_;
        return root;
    }
    Expression* parent() const { return parent_; }
    Expression* left() const { return left_; }
    Expression* right() const { return right_; }
    Type type() const { return type_; }
    const char* name() const { assert(type_ == VARIABLE); return name_; }
    double value() const { assert(type_ == CONSTANT); return value_; }
    double (*eval1() const)(double) { assert(type_ == FUNCTION1); return eval1_; }
    double (*eval2() const)(double,double) { assert(type_ == FUNCTION2); return eval2_; }

private:

    union {
        double value_;
        char* name_;
        double (*eval1_)(double);
        double (*eval2_)(double,double);
    };
    Expression* parent_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    Type type_;
};

}
