#include "tfp/math/SymbolicMatrix.hpp"
#include "tfp/util/Tears.hpp"
#include "tfp/math/ExpressionOptimiser.hpp"

#include <iostream>

namespace tfp {

// ----------------------------------------------------------------------------
void swap(tfp::SymbolicMatrix& a, tfp::SymbolicMatrix& b)
{
    using std::swap;

    swap(a.rows_, b.rows_);
    swap(a.columns_, b.columns_);
    swap(a.entries_, b.entries_);
}

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& stream, const tfp::SymbolicMatrix& m)
{
    for (int c = 0; c != m.columns(); ++c)
    {
        for (int r = 0; r != m.rows(); ++r)
        {
            stream << m.entry(r, c)->evaluate();
            if (r < m.rows() - 1)
                stream << " ";
        }
        if (c < m.columns() - 1)
            stream << "\n";
    }
    return stream;
}

}

using namespace tfp;

#define REQUIRE_SAME_SIZE(OTHER, RET) do { \
        if (rows() != OTHER.rows() || columns() != OTHER.columns()) { \
            g_tears.cry("Matrices must have the same size. Got %dx%d and %dx%d", rows(), columns(), OTHER.rows(), OTHER.columns()); \
            return RET; \
        } \
    } while(0)

#define REQUIRE_COLUMNS_EQUALS_ROWS_OF(OTHER, RET) do { \
        if (columns() != OTHER.rows()) { \
            g_tears.cry("Matrices have unequal column/rows. Got %dx%d and %dx%d, need m*n and l*o with n=l", rows(), columns(), OTHER.rows(), OTHER.columns()); \
            return RET; \
        } \
    } while(0)

#define REQUIRE_SQUARE(RET) do { \
        if (columns() != rows()) { \
            g_tears.cry("Matrix has to be square. Got %dx%d", rows(), columns()); \
            return RET; \
        } \
    } while(0)

// ----------------------------------------------------------------------------
SymbolicMatrix::SymbolicMatrix() :
    rows_(0),
    columns_(0)
{
}

// ----------------------------------------------------------------------------
SymbolicMatrix::SymbolicMatrix(int rows, int columns) :
    rows_(rows),
    columns_(columns),
    entries_(rows * columns)
{
}

// ----------------------------------------------------------------------------
SymbolicMatrix::SymbolicMatrix(const SymbolicMatrix& other) :
    rows_(other.rows_),
    columns_(other.columns_),
    entries_(rows_ * columns_)
{
    for (int i = 0; i != rows_*columns_; ++i)
        entries_[i] = other.entries_[i]->clone();
}

// ----------------------------------------------------------------------------
SymbolicMatrix& SymbolicMatrix::operator=(SymbolicMatrix other)
{
    swap(*this, other);
    return *this;
}

// ----------------------------------------------------------------------------
void SymbolicMatrix::resize(int rows, int columns)
{
    int size = rows * columns;
    ExpressionList target;
    target.resize(size);
    for (int row = 0; row != rows_; ++row)
        for (int column = 0; column != columns_; ++ column)
        {
            if (row >= rows || column >= columns)
                continue;
            target[row*columns+column] = entries_[row*columns_+column];
        }

    entries_ = target;
    rows_ = rows;
    columns_ = columns;
}

// ----------------------------------------------------------------------------
int SymbolicMatrix::rows() const
{
    return rows_;
}

// ----------------------------------------------------------------------------
int SymbolicMatrix::columns() const
{
    return columns_;
}

// ----------------------------------------------------------------------------
void SymbolicMatrix::fillZeros()
{
    for (int i = 0; i != rows_*columns_; ++i)
        entries_[i] = Expression::make(0.0);
}

// ----------------------------------------------------------------------------
void SymbolicMatrix::fillIdentity()
{
    for (int row = 0; row != rows_; ++row)
        for (int column = 0; column != columns_; ++column)
        {
            if (row == column)
                entries_[row*columns_+column] = Expression::make(1.0);
            else
                entries_[row*columns_+column] = Expression::make(0.0);
        }
}

// ----------------------------------------------------------------------------
void SymbolicMatrix::setEntry(int row, int column, const char* expression)
{
    entries_[row*columns_+column] = Expression::parse(expression);
}

// ----------------------------------------------------------------------------
void SymbolicMatrix::setEntry(int row, int column, Expression* e)
{
    entries_[row*columns_+column] = e;
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::entry(int row, int column) const
{
    return entries_[row*columns_+column];
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::add(const SymbolicMatrix& other)
{
    REQUIRE_SAME_SIZE(other, SymbolicMatrix());

    SymbolicMatrix ret(rows_, columns_);
    for (int i = 0; i != rows_*columns_; ++i)
    {
        ret.entries_[i] = Expression::make(op::add, entries_[i]->clone(), other.entries_[i]->clone());
        ret.entries_[i]->optimise();
    }

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::add(Expression* e)
{
    return applyOperationToEveryEntry(op::add, e);
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::sub(const SymbolicMatrix& other)
{
    REQUIRE_SAME_SIZE(other, SymbolicMatrix());

    SymbolicMatrix ret(rows_, columns_);
    for (int i = 0; i != rows_*columns_; ++i)
    {
        ret.entries_[i] = Expression::make(op::sub, entries_[i]->clone(), other.entries_[i]->clone());
        ret.entries_[i]->optimise();
    }

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::sub(Expression* e)
{
    return applyOperationToEveryEntry(op::sub, e);
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::mul(const SymbolicMatrix& other)
{
    REQUIRE_COLUMNS_EQUALS_ROWS_OF(other, SymbolicMatrix());

    SymbolicMatrix ret(rows_, other.columns_);
    for (int row = 0; row != rows_; ++row)
        for (int column = 0; column != other.columns_; ++column)
        {
            ret.entries_[row*other.columns_+column] = Expression::make(op::mul,
                    entries_[row*columns_]->clone(),
                    other.entries_[column]->clone());

            for (int off = 1; off < columns_; ++off)
            {
                ret.entries_[row*other.columns_+column] = Expression::make(op::add,
                        ret.entries_[row*other.columns_+column],
                        Expression::make(op::mul,
                                entries_[row*columns_+off]->clone(),
                                other.entries_[off*other.columns_+column]->clone()));
            }

            ret.entries_[row*other.columns_+column]->optimise();
        }

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::mul(Expression* e)
{
    return applyOperationToEveryEntry(op::mul, e);
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::div(const SymbolicMatrix& other)
{
    return mul(other.inverse());
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::div(Expression* e)
{
    return applyOperationToEveryEntry(op::div, e);
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::minorBlock(int row, int column) const
{
    SymbolicMatrix ret(rows_ - 1, columns_ - 1);

    /*
     * Copy the correct expressions into the minor matrix (skipping the
     * current column and row.
     */
    int i = 0;
    for (int r = 0; r != rows_; ++r)
    {
        if (r == row)
            continue;
        for (int c = 0; c != columns_; ++c)
        {
            if (c == column)
                continue;
            ret.entries_[i++] = entries_[r*columns_+c];
        }
    }

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::minorMatrix() const
{
    SymbolicMatrix ret(rows_, columns_);

    int i = 0;
    for (int r = 0; r != rows_; ++r)
        for (int c = 0; c != columns_; ++c)
            ret.entries_[i++] = minorBlock(r, c).determinant();

    return ret;
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::determinant() const
{
    REQUIRE_SQUARE(Expression::make(0.0));

    Expression* result = determinantNoOptimisation();
    result->optimise();
    return result;
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::determinantNoOptimisation() const
{
    switch (rows())
    {
        case 0 : return Expression::make(0.0);
        case 1 : return entries_[0]->clone(); break;
        case 2 : return determinant2x2();     break;
        case 3 : return determinant3x3();     break;
        default: return determinantNxN();     break;
    }
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::determinant2x2() const
{
    return Expression::make(op::sub,
            Expression::make(op::mul,
                entries_[0]->clone(),
                entries_[3]->clone()),
            Expression::make(op::mul,
                entries_[1]->clone(),
                entries_[2]->clone()));
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::determinant3x3() const
{
    /*
     * Given the matrix
     *
     *      / a b c \
     *  A = | d e f |
     *      \ g h i /
     *
     * The determinant can be calculated with:
     *
     *  det(A) = (aei + bfg + cdh) - (gec + hfa + idb)
     *
     * The entries in the matrix are stored such that a=0, b=1, c=2, etc.
     */
    return Expression::make(op::sub,                   // (aei + bfg + cdh) - (gec + hfa + idb)
            Expression::make(op::add,                  // aei + bfg + cdh
                Expression::make(op::add,              // aei + bfg
                    Expression::make(op::mul,          // aei
                        Expression::make(op::mul,      // ae
                            entries_[0]->clone(),      // a
                            entries_[4]->clone()),     // e
                        entries_[8]->clone()),         // i
                    Expression::make(op::mul,          // bfg
                        Expression::make(op::mul,      // bf
                            entries_[1]->clone(),      // b
                            entries_[5]->clone()),     // f
                        entries_[6]->clone())),        // g
                Expression::make(op::mul,              // cdh
                    Expression::make(op::mul,          // cd
                        entries_[2]->clone(),          // c
                        entries_[3]->clone()),         // d
                    entries_[2*3+1]->clone())),        // h
            Expression::make(op::add,                  // gec + hfa + idb
                Expression::make(op::add,              // gec + hfa
                    Expression::make(op::mul,          // gec
                        Expression::make(op::mul,      // ge
                            entries_[6]->clone(),      // g
                            entries_[4]->clone()),     // e
                        entries_[2]->clone()),         // c
                    Expression::make(op::mul,          // hfa
                        Expression::make(op::mul,      // hf
                            entries_[7]->clone(),      // h
                            entries_[5]->clone()),     // f
                        entries_[0]->clone())),        // a
                Expression::make(op::mul,              // idb
                    Expression::make(op::mul,          // id
                        entries_[8]->clone(),          // i
                        entries_[3]->clone()),         // d
                    entries_[1]->clone())));           // b
}

// ----------------------------------------------------------------------------
Expression* SymbolicMatrix::determinantNxN() const
{
    Expression* det = Expression::make(op::mul,
            entry(0, 0),
            minorBlock(0, 0).determinantNoOptimisation());
    SymbolicMatrix minorM(rows_ - 1, columns_ - 1);
    op::Op2 toggleOp = op::sub;
    for (int column = 1; column != columns_; ++column)
    {
        /*
         * By multiplying the skipped entry in row 0 with the determinant of
         * the minor matrix and summing all of those results together, we
         * obtain the determinant of the entire matrix.
         */
        det = Expression::make(toggleOp,
                det,
                Expression::make(op::mul,
                                 entry(0, column),
                                 minorBlock(0, column).determinantNoOptimisation()));

        // Subdeterminants alternates between adding and subtracting
        toggleOp = toggleOp == op::add ? op::sub : op::add;
    }

    det->optimise();
    return det;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::transpose() const
{
    if (rows_ == 1 || columns_ == 1)
    {
        SymbolicMatrix ret = *this;
        std::swap(ret.rows_, ret.columns_);
        return ret;
    }

    SymbolicMatrix ret(rows_, columns_);
    for (int r = 0; r != rows_; ++r)
        for (int c = 0; c != columns_; ++c)
            ret.entries_[r*columns_+c] = entries_[c*columns_+r]->clone();


    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::adjugate() const
{
    SymbolicMatrix ret = minorMatrix().transpose();

    /*
     * Need to apply "checkerboard" pattern of additions and subtractions to
     * get the matrix of cofactors. This can be done after transposing, since
     * transposing doesn't change the checkerpattern.
     */
    for (int r = 0; r < ret.rows_; ++r)
        for (int c = (r+1)%2; c < ret.rows_; c += 2)
            ret.entries_[r*ret.columns_+c] = Expression::make(
                    op::negate, ret.entries_[r*ret.columns_+c]);

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::inverse() const
{
    REQUIRE_SQUARE(SymbolicMatrix());

    SymbolicMatrix result;
    switch (rows())
    {
        case 0 : return SymbolicMatrix();
        case 1 : result.resize(1, 1);
                 result.setEntry(0, 0, Expression::make(op::div, Expression::make(1.0), entries_[0]->clone()));
                 break;
        case 2 : result = inverse2x2(); break;
        case 3 : result = inverse3x3(); break;
        default: result = inverseNxN(); break;
    }

    for (int i = 0; i != rows_*columns_; ++i)
        result.entries_[i]->optimise();

    return result;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::inverse2x2() const
{
    SymbolicMatrix ret(2, 2);

    Expression* det = determinantNoOptimisation();
    ret.entries_[0] = Expression::make(op::div, entries_[3]->clone(), det); // Not cloning det on purpose, otherwise det won't have a reference and won't be deleted
    ret.entries_[1] = Expression::make(op::div, Expression::make(op::negate, entries_[1]->clone()), det->clone());
    ret.entries_[2] = Expression::make(op::div, Expression::make(op::negate, entries_[2]->clone()), det->clone());
    ret.entries_[3] = Expression::make(op::div, entries_[0]->clone(), det->clone());
    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::inverse3x3() const
{
    SymbolicMatrix ret(3, 3);

    // Generates the expression "ab - cd" where a, b, c, d and indices into entries_
#define ENTRY_EXPR(a,b,c,d)             \
        Expression::make(op::sub,       \
            Expression::make(op::mul,   \
                entries_[a]->clone(),   \
                entries_[b]->clone()),  \
            Expression::make(op::mul,   \
                entries_[c]->clone(),   \
                entries_[d]->clone()))

    Expression* det = determinantNoOptimisation();
    ret.entries_[0] = Expression::make(op::div, ENTRY_EXPR(4, 8, 5, 7), det); // Not cloning det on purpose, otherwise det won't have a reference and won't be deleted
    ret.entries_[1] = Expression::make(op::div, ENTRY_EXPR(7, 2, 8, 1), det->clone());
    ret.entries_[2] = Expression::make(op::div, ENTRY_EXPR(1, 5, 2, 4), det->clone());
    ret.entries_[3] = Expression::make(op::div, ENTRY_EXPR(6, 5, 3, 8), det->clone());
    ret.entries_[4] = Expression::make(op::div, ENTRY_EXPR(0, 8, 6, 2), det->clone());
    ret.entries_[5] = Expression::make(op::div, ENTRY_EXPR(3, 2, 0, 5), det->clone());
    ret.entries_[6] = Expression::make(op::div, ENTRY_EXPR(3, 7, 6, 4), det->clone());
    ret.entries_[7] = Expression::make(op::div, ENTRY_EXPR(6, 1, 0, 7), det->clone());
    ret.entries_[8] = Expression::make(op::div, ENTRY_EXPR(0, 4, 3, 1), det->clone());

    for (int i = 0; i != 9; ++i)
        ret.entries_[i]->optimise();

    return ret;
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::inverseNxN() const
{
    SymbolicMatrix mm = minorMatrix();
    SymbolicMatrix ret = mm.transpose();

    /*
     * Need to apply "checkerboard" pattern of additions and subtractions to
     * get the matrix of cofactors. This can be done after transposing, since
     * transposing doesn't change the checkerpattern.
     */
    for (int r = 0; r < ret.rows_; ++r)
        for (int c = (r+1)%2; c < ret.rows_; c += 2)
            ret.entries_[r*ret.columns_+c] = Expression::make(
                    op::negate, ret.entries_[r*ret.columns_+c]);

    /*
     * Determinant can be calculated from the minor matrix to save some time.
     */
    Reference<Expression> det = Expression::make(op::mul, entry(0, 0), mm.entry(0, 0));
    op::Op2 toggleOp = op::sub;
    for (int column = 1; column != columns_; ++column)
    {
        det = Expression::make(toggleOp,
                               det,
                               Expression::make(op::mul,
                                                entry(0, column),
                                                mm.entry(0, column)));
        toggleOp = toggleOp == op::add ? op::sub : op::add;
    }

    det->optimise();
    return ret.div(det);
}

// ----------------------------------------------------------------------------
SymbolicMatrix SymbolicMatrix::applyOperationToEveryEntry(op::Op2 operation, Expression* e)
{
    SymbolicMatrix ret(rows_, columns_);
    for (int i = 0; i != rows_*columns_; ++i)
    {
        ret.entries_[i] = Expression::make(operation, entries_[i]->clone(), e->clone());
        ret.entries_[i]->optimise();
    }

    return ret;
}
