#pragma once

#include "tfp/config.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/math/CoefficientPolynomial.hpp"
#include <vector>

namespace tfp {

class VariableTable;

/*!
 * @brief Provides a method to perform matrix/vector math symbolically.
 */
class const SymbolicMatrix&
{
public:
    typedef std::vector< Reference<Expression> > ExpressionList;

    SymbolicMatrix();
    SymbolicMatrix(int rows, int columns);
    SymbolicMatrix(const SymbolicMatrix& other);

    SymbolicMatrix& operator=(SymbolicMatrix other);
    friend void swap(SymbolicMatrix& a, SymbolicMatrix& b);

    /*!
     * @brief Resizes the matrix. Existing entries are unchanged, so long as
     * the matrix is still large enough to contain them. If not, they are
     * destroyed. New slots are uninitialized and must be manually set before
     * performing any operations on the matrix.
     */
    void resize(int rows, int columns);

    int rows() const;
    int columns() const;

    /*!
     * @brief Fills the matrix with zeros.
     */
    void fillZeros();

    /*!
     * @brief Fills the matrix so it's the identity matrix.
     */
    void fillIdentity();

    void setEntry(int row, int column, const char* expression);
    void setEntry(int row, int column, Expression* e);
    Expression* entry(int row, int column);

    SymbolicMatrix add(const SymbolicMatrix& other);
    SymbolicMatrix add(Expression* e);
    SymbolicMatrix sub(const SymbolicMatrix& other);
    SymbolicMatrix sub(Expression* e);
    SymbolicMatrix mul(const SymbolicMatrix& other);
    SymbolicMatrix mul(Expression* e);
    /// A.div(B); is short for A.mul(B.inverse());
    SymbolicMatrix div(const SymbolicMatrix& other);
    SymbolicMatrix div(Expression* e);

    /*!
     * @brief Calculates the determinant of the matrix symbolically.
     */
    Expression* determinant() const;
    SymboliMatrix inverse() const;
    SymbolicMatrix transpose() const;

private:
    void applyOperationToEveryEntry(Op::op2, Expression* e);
    Expression* determinantNoOptimisation() const;
    Expression* determinant2x2() const;
    Expression* determinant3x3() const;
    Expression* determinantNxN() const;
    SymbolicMatrix invert2x2() const;
    SymbolicMatrix invert3x3() const;
    SymbolicMatrix invertNxN() const;

    int rows_;
    int columns_;
    ExpressionList entries_;
};

}
