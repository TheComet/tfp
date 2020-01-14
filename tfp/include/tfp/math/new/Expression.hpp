#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"

namespace tfpnew {

namespace op {

typedef double (*Op1)(double);
typedef double (*Op2)(double,double);

// Unary operations
TFP_PUBLIC_API double negate(double a);

// Binary operations
TFP_PUBLIC_API double add(double a, double b);
TFP_PUBLIC_API double sub(double a, double b);
TFP_PUBLIC_API double mul(double a, double b);
TFP_PUBLIC_API double div(double a, double b);
TFP_PUBLIC_API double pow(double a, double b);
TFP_PUBLIC_API double mod(double a, double b);
TFP_PUBLIC_API double comma(double a, double b);

}

using namespace tfp;

/*!
 * @brief A symbolic representation of a mathematical expression.
 *
 * The expression can consist of constants, variables, or functions, and can be
 * evaluated to an actual value
 *
 * Some notes on design decisions:
 *   - It was preferable to dedicate the static function Expression::make() for
 *     allocating and initialising an expression object instead of overloading
 *     the constructor. The reason was because both Expression::parse() and
 *     Expression::make() take strings, but do very different things. Having a
 *     constructor that takes a sting makes its purpose a little ambiguous.
 */
class TFP_PUBLIC_API Expression : public RefCounted
{
public:
    enum Type
    {
        INVALID,
        CONSTANT,
        INF,
        VARIABLE,
        FUNCTION1,
        FUNCTION2
    };
    
    /*! May need to free the name if the type is a variable */
    ~Expression();
    
    /*!
     * @brief Parses a string into a syntax tree.
     * @return If parsing fails, NULL is returned.
     */
    static Expression* parse(const char* expression);
    static int yyInput(char* buf, int buflen);
    
    /*!
     * @brief Instantiates a new node and marks it as invalid.
     */
    static Expression* makeInvalid();

    /*!
     * @brief Instantiates a new node with the given variable name.
     * @note This should not be confused with Expression::parse() -- this only
     * allocates a single node in the syntax tree.
     * @param[in] variableName A string containing the name of the variable.
     * @return Returns a new expression object of type VARIABLE.
     */
    static Expression* make(const char* variableName);

    /*!
     * @brief Instantiates a new node with the given value.
     * @param[in] value The numeric value the node should represent.
     * @return Returns a new expression object of type CONSTANT.
     */
    static Expression* make(double value);

    /*!
     * @brief Instantiates a new node representing a single-operand operation.
     * @param[in] func The operation this node should perform on its child.
     * This is stored as a pointer to a function of type op::Op1, i.e.
     * ```double func(double value)```. For example, ```op::negate```.
     * @param[in] rhs The child expression this operation should be performed
     * on. Even though Expression objects are refcounted, you should think of
     * the parent expression taking ownership of rhs.
     * @return Returns a new expression object of type FUNCTION1.
     * @note The child is stored as the RHS operand and can be retrieved again
     * with Expression::right(). The LHS will return NULL.
     * @note It is possible to pass an existing expression into itself, e.g.
     * ```cpp
     * Reference<Expression> a = Expression::make("a");
     * a = Expression::make(op::negate, a);
     * ```
     */
    static Expression* make(op::Op1 func, Expression* rhs);

    /*!
     * @brief Instantiates a new node representing a double-operand operation
     * such as addition, subtraction, division, etc.
     * @param[in] func The operation this node should perform on its left and
     * right hand side children. This is stored as a pointer to a function of
     * type op::Op2, i.e. ```double func(double value)```. Examples are
     * ```op::add```, ```op::mul```, ```op::pow```, etc.
     * @param[in] lhs The left-hand-side (LHS) expression this operation should
     * be performed on.
     * @param[in] rhs The right-hand-side (RHS) expression this operation
     * should be performed on.
     * @note Even though Expression objects are refcounted, you should think of
     * the parent expression taking ownership of both LHS and RHS expressions.
     * @return Returns a new expression object of type FUNCTION2.
     * @note It is possible to pass an existing expression into itself, e.g.
     * ```cpp
     * Reference<Expression> a = Expression::make("a");
     * Reference<Expression> b = Expression::make("b");
     * a = Expression::make(op::add, a, b);
     * ```
     */
    static Expression* make(op::Op2 func, Expression* lhs, Expression* rhs);

    /*!
     * @brief Instantiates a new node representing infinity.
     * @return Returns a new expression object of type INF.
     */
    static Expression* makeInfinity();
    
    /*!
     * @brief Resets all data in this node, but keeps its relations (children
     * and parent) in tact. The type is set to INVALID.
     */
    void setInvalid();

    /*!
     * @brief Sets this node's variable name and sets the type to VARIABLE.
     * @param[in] variableName A string containing the name of the variable.
     */
    void set(const char* variableName);

    /*!
     * @brief Sets this node's value and sets the type to CONSTANT.
     * @param[in] value The numeric value the node should represent.
     */
    void set(double value);

    /*!
     * @brief Sets this node to a single-operand operation, type FUNCTION1.
     * @param[in] func The operation this node should perform on its child.
     * This is stored as a pointer to a function of type op::Op1, i.e.
     * ```double func(double value)```. For example, ```op::negate```.
     * @param[in] rhs The child expression this operation should be performed
     * on. Even though Expression objects are refcounted, you should think of
     * the parent expression taking ownership of rhs.
     * @note The child is stored as the RHS operand and can be retrieved again
     * with Expression::right(). The LHS will return NULL.
     */
    void set(op::Op1 func, Expression* rhs);

    /*!
     * @brief Sets this node to a double-operand operation, type FUNCTION2.
     * @param[in] func The operation this node should perform on its left and
     * right hand side children. This is stored as a pointer to a function of
     * type op::Op2, i.e. ```double func(double value)```. Examples are
     * ```op::add```, ```op::mul```, ```op::pow```, etc.
     * @param[in] lhs The left-hand-side (LHS) expression this operation should
     * be performed on.
     * @param[in] rhs The right-hand-side (RHS) expression this operation
     * should be performed on.
     * @note Even though Expression objects are refcounted, you should think of
     * the parent expression taking ownership of both LHS and RHS expressions.
     * ```
     */
    void set(op::Op2 func, Expression* lhs, Expression* rhs);

    /*!
     * @brief Sets this node's type to INF. All children are set to NULL.
     */
    void setInfinity();
    
    /*!
     * @brief Deep-copies the syntax tree. The root's parent will be set to NULL.
     * @param parent Always set to NULL.
     */
    Expression* clone(Expression* parent=NULL) const;
    
    /*!
     * @brief Copies the data and type from another expression object,
     * essentially transforming this expression into whatever the specified
     * other expression is.
     * @note This does not change this expression's relations within the syntax
     * tree (left(), right() and parent()) remain unchanged).
     * @note Never copy data from another expression that has a different type
     * than your own. Otherwise you might end up with constants that have
     * LHS and RHS nodes, or worse, operations that have no children. The one
     * exception is if you are marked as INVALID, then you can copy from any
     * other node you want. In debug mode, this is asserted.
     */
    void copyDataFrom(const Expression* other);
    
    /*!
     * @brief Does the exact same thing as Expression::copyDataFrom(), but
     * additionally marks the other node INVALID.
     * @param[in,out] other The expression to steal data from. Under the hood,
     * you gain a slight performance increase if the type is VARIABLE, because
     * in that case, the string is moved instead of copied and no allocations
     * are performed.
     */
    void stealDataFrom(Expression* other);
    
    /*!
     * @brief Counts all nodes in the syntax tree.
     */
    int countNodes() const;
    
#ifdef DEBUG
    /*!
     * @brief Traverses all nodes and verifies that their parents are pointing
     * to them. The refcounts are also asserted.
     */
    bool checkConsistencies() const;
#endif
    
    Expression* parent() const { return parent_; }
    Expression* left() const { return left_; }
    Expression* right() const { return right_; }
    Type type() const { return type_; }
    const char* name() const { assert(type_ == VARIABLE); return data_.name_; }
    double value() const { assert(type_ == CONSTANT); return data_.value_; }
    op::Op1 op1() const { assert(type_ == FUNCTION1); return data_.op1_; }
    op::Op2 op2() const { assert(type_ == FUNCTION2); return data_.op2_; }
    
private:
    Expression();
    
private:
    union {
        double value_;
        char* name_;
        op::Op1 op1_;
        op::Op2 op2_;
    } data_;
    Expression* parent_;
    Reference<Expression> left_;
    Reference<Expression> right_;
    Type type_;
    
    static const char* yyString_;
    static int yyOffset_;
};

}
