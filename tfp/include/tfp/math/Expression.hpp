#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/math/TransferFunction.hxx"
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

namespace tfp {

namespace op {

typedef double (*Op1)(double);
typedef double (*Op2)(double,double);
TFP_PUBLIC_API double add(double a, double b);
TFP_PUBLIC_API double sub(double a, double b);
TFP_PUBLIC_API double mul(double a, double b);
TFP_PUBLIC_API double div(double a, double b);
TFP_PUBLIC_API double pow(double a, double b);
TFP_PUBLIC_API double mod(double a, double b);
TFP_PUBLIC_API double negate(double a);
TFP_PUBLIC_API double comma(double a, double b);

}

class VariableTable;

class TFP_PUBLIC_API Expression : public tfp::RefCounted
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

    struct TransferFunctionCoefficients
    {
        typedef std::vector< tfp::Reference<Expression> > Coefficients;
        Coefficients numeratorCoefficients_;
        Coefficients denominatorCoefficients_;
        std::string argumentVariable_;
    };

    Expression();
    ~Expression();

    static Expression* parse(const char* expression);

    static Expression* make(const char* variableName);
    static Expression* make(double value);
    static Expression* make(op::Op1 func, Expression* rhs);
    static Expression* make(op::Op2 func, Expression* lhs, Expression* rhs);

    Expression* shallowClone();
    // deep copy, except parent defaults to NULL
    Expression* clone(Expression* parent=NULL);

    void set(const char* variableName);
    void set(double value);
    void set(op::Op1 func, Expression* rhs);
    void set(op::Op2 func, Expression* lhs, Expression* rhs);
    void set(Expression* other);
    void reset();
    
    Expression* find(const char* variableName);
    Expression* find(double value);
    Expression* find(op::Op1 func);
    Expression* find(op::Op2 func);

    VariableTable* generateVariableTable() const;
    double evaluate(const VariableTable* vt=NULL, std::set<std::string>* visited=NULL) const;
    
    bool recursivelyCall(bool (Expression::*optfunc)());
    bool recursivelyCall(bool (Expression::*optfunc)(const char*), const char* variable);

    /*!
     * Returns true if the types match and (depending on type) symbol names, value, or
     * operations match. Invalid expressions return false.
     */
    bool isSameAs(Expression* other) const;
    bool isOperation(op::Op1 func) const;
    bool isOperation(op::Op2 func) const;
    bool hasRHSOperation(op::Op1 func) const;
    bool hasRHSOperation(op::Op2 func) const;
    bool hasVariable(const char* variable) const;
    Expression* getOtherOperand() const;

    void dump(const char* filename, bool append=false);
    void dump(FILE* fp);
    void dump();

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

    // Expression_manipulation.cpp
    void enforceProductLHS(const char* variable);
    bool enforceConstantExponent(const char* variable);
    bool expandConstantExponentsIntoProducts(const char* variable);
    bool factorNegativeExponents(const char* variable);
    bool eliminateDivisionsAndSubtractions(const char* variable);
    bool eliminateNegativeExponents(const char* variable);
    Expression* findOrAddLatestDivision();
    bool expand(const char* variable);
    bool manipulateIntoRationalFunction(const char* variable);

    /*!
     * @brief Attempts to manipulate the expression into the standard "Transfer
     * function form" and returns two arrays containing the expressions for
     * each coefficient of the numerator and denominator polynomials.
     *
     *          b0 + b1*s^1 + b2*s^2 + ...
     *   T(s) = --------------------------
     *          a0 + a1*s^1 + a2*s^2 + ...
     *
     * There are all sorts of reasons why this might fail, but if it succeeds,
     * it means you have expressions for all of the polynomial coefficients,
     * which allows you to change variables and re-evaluate the new coefficient
     * values without needing to do this transformation each time. The
     * coefficients are required to build a TransferFunction object, which is
     * required for calculating impulse/step responses, frequency responses,
     * pole-zero diagrams, etc.
     *
     * If this function doesn't succeed, which will be the case if any term
     * that contains the specified variable (usually "s") is combined with an
     * operation that is *not* mul/div/add/sub with a variable operand (such as
     * the expression s^a) -- or in other words, if the operation containing
     * the specified variable is not reducible to polynomial form -- the
     * expression will be in a modified state different from the initial
     * condition, but mathematically equivalent. In this case, no expressions
     * exist for the polynomial coefficients and the transfer function will
     * have to perform this manipulation every time a variable changes.
     *
     * It is worth noting that DPSFGs will always produce expressions reducible
     * to polynomial expressions. User input may not.
     */
    TransferFunctionCoefficients calculateTransferFunctionCoefficients(const char* variable);

    /*!
     * @brief Evaluates all coefficient expressions and constructs a transfer
     * function.
     */
    tfp::TransferFunction<double> calculateTransferFunction(const TransferFunctionCoefficients& tfe,
                                                            const VariableTable* vt=NULL);

    // Expression_optimisation.cpp
    void optimise();
    bool optimiseMultipleNegates();
    bool optimiseConstantExpressions();
    bool optimiseUselessAdditions();
    bool optimiseUselessSubtractions();
    bool optimiseUselessProducts();
    bool optimiseUselessDivisions();
    bool optimiseUselessExponents();
    bool collapseChainOfOperations();
    bool optimiseExponentiate();
    bool optimiseAdditionsIntoProducts();
    bool checkParentConsistencies() const;

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
