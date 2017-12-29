#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <string>
#include <set>

namespace tfp {

class VariableTable;

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
class TFP_PUBLIC_API Expression : public tfp::RefCounted
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

    Expression();
    ~Expression();

    /*!
     * @brief Parses a string into a syntax tree.
     * @return If parsing fails, NULL is returned.
     */
    static Expression* parse(const char* expression);

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
     * @brief Replaces our position in the tree with another node and unlinks
     * us from the tree.
     * @param[in] other The node to take our place.
     * @return Returns this.
     * @warning The returned node has a refcount of 0 but will not be deleted.
     * This function assumes you are going to re-use the unlinked node
     * again. If not, make sure to reference it briefly so it gets cleaned up
     * (or just delete it).
     */
    Expression* unlinkExchange(Expression* other);

    /*!
     * @brief Steals "other's" data and position in the tree.
     */
    void replaceWith(Expression* other);

    /*!
     * @brief Removes the node from the tree without deleting it.
     * @note The children of this node will remain in tact.
     * @return Returns this.
     * @warning The returned node has a refcount of 0 but will not be deleted.
     * This function assumes you are going to re-use the unlinked node
     * again. If not, make sure to reference it briefly so it gets cleaned up
     * (or just delete it).
     */
    Expression* unlinkFromTree();

    /*!
     * @brief Deep-copies the syntax tree. The root's parent will be set to NULL.
     * @param parent Always set to NULL.
     */
    Expression* clone(Expression* parent=NULL) const;

    /*!
     * @brief The left and right operands are exchanged.
     * @note This operation is only valid for FUNCTION2 types.
     */
    void swapOperands();

    /*!
     * @brief Causes this node to "overwrite" its parent. That is, the parent
     * node will adopt all data from this node, including children.
     */
    void collapseIntoParent();

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
     * @brief Resets all data in this node, but keeps its relations (children
     * and parent) in tact. The type is set to INVALID.
     */
    void reset();

    /*!
     * @brief Performs all available optimisations on the tree.
     */
    bool optimise();

    struct RecurseAll { bool operator()(const Expression* e) const {
        return e != NULL;
    }};
    template <op::Op2 op>
    struct RecurseOnOp2 { bool operator()(const Expression* e) const {
        return e->type() == FUNCTION2 && e->op2() == op ? true : false;
    }};

    struct MatchAll { bool operator()(const Expression* e) const {
        return true;
    }};
    template <Type type>
    struct MatchOperandType { bool operator()(const Expression* e) const {
        return e->left()->type() == type || e->right()->type() == type ? true : false;
    }};
    struct MatchSame {
        MatchSame(const Expression* other) : other_(other) {}
        bool operator()(const Expression* e) const {
            if (e->isSameAs(other_))
                return true;
            return false;
        }
        const Expression* other_;
    };
    struct MatchValue {
        MatchValue(double value) : value_(value) {}
        bool operator()(const Expression* e) const {
            if (e->type() == CONSTANT && e->value() == value_)
                return true;
            return false;
        }
        const double value_;
    };
    struct MatchVariable {
        MatchVariable(const char* variableName) : variableName_(variableName) {}
        bool operator()(const Expression* e) const {
            if (e->type() == VARIABLE && strcmp(e->name(), variableName_) == 0)
                return true;
            return false;
        }
        const char* variableName_;
    };
    struct MatchOp1 {
        MatchOp1(const op::Op1& op) : op_(op) {}
        bool operator()(const Expression* e) const {
            if (e->type() == FUNCTION1 && e->op1() == op_)
                return true;
            return false;
        }
        const op::Op1 op_;
    };
    struct MatchOp2 {
        MatchOp2(const op::Op2& op) : op_(op) {}
        bool operator()(const Expression* e) const {
            if (e->type() == FUNCTION2 && e->op2() == op_)
                return true;
            return false;
        }
        const op::Op2 op_;
    };

    /*!
     * @brief Search for a node in the syntax tree, with an option to ignore
     * a specific node.
     *
     * Use the template parameters to custimize what to search for. The first
     * template parameter is a match functor. find() will return the first node
     * upon which the matcher returns true. By default, all nodes are matched.
     * The second template parameter is the recurse condition. find() will
     * recurse into children only when this functor returns true. The default
     * is to recurse into all children.
     *
     * @param[in] ignore An optional node to ignore.
     * @return Returns NULL if nothing was found, or an expression node if
     * a match was found.
     * @note The root object is considered in this search.
     */
    template <class Matcher, class RecurseCondition=RecurseAll>
    Expression* find(const Matcher& match,
                     const RecurseCondition& recurse=RecurseCondition(),
                     Expression* ignore=NULL)
    {
        Expression* e;

        if (this == ignore)
            return NULL;

        if (match(this)) return this;
        if (recurse(left()) && (e = left()->find(match, recurse, ignore)) != NULL)
            return e;
        if (recurse(right()) && (e = right()->find(match, recurse, ignore)) != NULL)
            return e;
        return NULL;
    }

    Expression* find(double value);
    Expression* find(const char* variable);
    Expression* find(op::Op1 func);
    Expression* find(op::Op2 func);

    /*!
     * Recursively finds an expression node of type FUNCTION2 that has at least
     * one operand that matches the specified type. If ignore is not NULL,
     * then the returned expression is guaranteed not to be ignore.
     */
    Expression* findSame(const Expression* match);
    Expression* findOpWithSameLHS(op::Op2 func, const Expression* match);
    Expression* findOpWithNegativeRHS(op::Op2 func);
    Expression* travelUpChain(op::Op2 func);
    Expression* findSameDownChain(op::Op2 func, const Expression* match);

    Expression* travelUpComplementaryChain(op::Op2 func1, op::Op2 func2, double* negated);
    Expression* findSameDownComplementaryChain(op::Op2 func1, op::Op2 func2, double* negated, Expression* match);

    /*!
     * @brief Collects all variables in the expression into a new variable
     * table.
     * @note The variables are mapped to default values of either 1.0 if the
     * parent operation is mul, div or pow, or 0.0 if anything else.
     */
    VariableTable* generateVariableTable() const;

    /*!
     * @brief Tries to resolve as many variable names as possible in the
     * expression using the specified variable table. If a variable is not
     * found in the variable table then it will remain unresolved.
     * @note The expression is not optimized after this operation, you may want
     * to call Expression::optimise().
     * @return Returns false if a cyclic dependency exists. In this case the
     * expression is left in a modified state. Returns true otherwise.
     */
    bool insertSubstitutions(const VariableTable* vt);

    /*!
     * @brief Evaluates the syntax tree and computes a numerical value.
     * @param[in] vt Optional variable table. This is required if your tree has
     * variables in it. If it does not, then you can pass NULL here.
     * @param[in] visited Leave NULL.
     * @return If anything goes wrong, NaN is returned.
     */
    double evaluate(const VariableTable* vt=NULL, std::set<std::string>* visited=NULL) const;

    bool checkParentConsistencies() const;

    /*!
     * Returns true if the types match and (depending on type) symbol names, value, or
     * operations match. Invalid expressions return false.
     */
    bool isSameAs(const Expression* other) const;
    bool isOperation(op::Op1 func) const;
    bool isOperation(op::Op2 func) const;
    bool hasRHSOperation(op::Op1 func) const;
    bool hasRHSOperation(op::Op2 func) const;
    bool hasVariable(const char* variable) const;

    int size() const;

    Expression* getOtherOperand() const;

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
    const char* name() const { assert(type_ == VARIABLE); return data_.name_; }
    double value() const { assert(type_ == CONSTANT); return data_.value_; }
    op::Op1 op1() const { assert(type_ == FUNCTION1); return data_.op1_; }
    op::Op2 op2() const { assert(type_ == FUNCTION2); return data_.op2_; }

    void dump(const char* filename, bool append=false, const char* context="");
    void dump(FILE* fp, const char* context="");
    void dump();

private:

    union {
        double value_;
        char* name_;
        op::Op1 op1_;
        op::Op2 op2_;
    } data_;
    Expression* parent_;
    tfp::Reference<Expression> left_;
    tfp::Reference<Expression> right_;
    Type type_;
};

}
