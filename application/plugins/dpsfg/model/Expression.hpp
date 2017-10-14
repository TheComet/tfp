#pragma once

#include "tfp/util/Reference.hpp"
#include <string>
#include <unordered_map>

namespace dpsfg {

namespace op {

typedef double (*Op1)(double);
typedef double (*Op2)(double,double);
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

    static Expression* make(const char* variableName);
    static Expression* make(double value);
    static Expression* make(op::Op1 func, Expression* rhs);
    static Expression* make(op::Op2 func, Expression* lhs, Expression* rhs);

    Expression* clone() const;

    void set(const char* variableName);
    void set(double value);
    void set(op::Op1 func, Expression* rhs);
    void set(op::Op2 func, Expression* lhs, Expression* rhs);
    void set(const Expression* other);
    void reset();

    VariableTable* generateVariableTable() const;
    double evaluate(const VariableTable* vt=NULL) const;

    bool isOperation(op::Op1 func) const;
    bool isOperation(op::Op2 func) const;
    bool hasRHSOperation(op::Op1 func) const;
    bool hasRHSOperation(op::Op2 func) const;
    bool hasVariable(const char* variable) const;
    void reorderProducts(const char* variable);
    bool eliminateDivisionsAndSubtractions(const char* variable);
    bool eliminateNegativeExponents(const char* variable);
    void optimise();
    void optimiseOperations();
    void optimiseConstants();

    bool manipulateIntoRationalFunction(const char* variable);

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
    op::Op1 op1() const { assert(type_ == FUNCTION1); return op1_; }
    op::Op2 op2() const { assert(type_ == FUNCTION2); return op2_; }

private:

    union {
        double value_;
        char* name_;
        op::Op1 op1_;
        op::Op2 op2_;
    };
    Expression* parent_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    Type type_;
};

}
