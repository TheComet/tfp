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
class SymbolicMatrix
{
public:
    typedef std::vector< Reference<Expression> > ExpressionList;

    SymbolicMatrix();
    SymbolicMatrix(const SymbolicMatrix& other);

    template <class... T>
    SymbolicMatrix(int rows, int columns, T&&... entries) :
        rows_(rows),
        columns_(columns),
        entries_(rows*columns)
    {
        setEntries(std::forward<T>(entries)...);
    }

    SymbolicMatrix(int rows, int columns);

    SymbolicMatrix& operator=(SymbolicMatrix other);

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

    template <class A, class... B>
    void setEntries(A&& arg, B&&... args)
    {
        entries_[rows_*columns_ - sizeof...(B) - 1] = Expression::make(arg);
        setEntries(std::forward<B>(args)...);
    }

    template <class A>
    void setEntries(A&& arg)
    {
        entries_[rows_*columns_ - 1] = Expression::make(arg);
    }

    void setEntry(int row, int column, const char* expression);
    void setEntry(int row, int column, Expression* e);
    void setEntry(int row, int column, double value);
    Expression* entry(int row, int column) const;

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
     * @brief Returns an n-1 by n-1 matrix omitting the specified row and
     * column.
     *
     * Example for row=1 and column=1:
     *
     *  /a b c d\     /a | c d\
     *  |e f g h|     |--+----|     /a c d\
     *  |i j k l| --> |i | k l| --> |i k l|
     *  \m n o p/     \m | o p/     \m o p/
     *
     * @warning This does **not** clone() the expressions. Changing any
     * expression in the returned matrix will thus also change entries in the
     * original matrix.
     */
    SymbolicMatrix minorBlock(int row, int column) const;

    /*!
     * @brief Computes the matrix of minors. That is, the resulting matrix will
     * contain the determinant of every minor block.
     */
    SymbolicMatrix minorMatrix() const;

    SymbolicMatrix transpose() const;

    SymbolicMatrix adjugate() const;

    /*!
     * @brief Calculates the determinant of the matrix symbolically.
     */
    Expression* determinant() const;
    SymbolicMatrix inverse() const;

    friend void swap(SymbolicMatrix& a, SymbolicMatrix& b);
    friend std::ostream& operator<<(std::ostream& stream, const SymbolicMatrix& m);

private:
    SymbolicMatrix applyOperationToEveryEntry(op::Op2, Expression* e);
    Expression* determinantNoOptimisation() const;
    Expression* determinant2x2() const;
    Expression* determinant3x3() const;
    Expression* determinantNxN() const;
    SymbolicMatrix inverse2x2() const;
    SymbolicMatrix inverse3x3() const;
    SymbolicMatrix inverseNxN() const;

    int rows_;
    int columns_;
    ExpressionList entries_;
};

}

