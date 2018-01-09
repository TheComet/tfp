#include "gmock/gmock.h"
#include "tfp/math/SymbolicMatrix.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/listeners/TearListener.hpp"
#include "tfp/util/Tears.hpp"

#define NAME SymbolicMatrix

using namespace testing;
using namespace tfp;

struct MockTearListener : public TearListener
{
    MockTearListener() { g_tears.dispatcher.addListener(this); }
    ~MockTearListener() { g_tears.dispatcher.removeListener(this); }
    MOCK_METHOD1(onTears, void(const char*));
};

static SymbolicMatrix generateSymMatrix(int rows, int columns)
{
    SymbolicMatrix m(rows, columns);
    static const char* syms = "abcdefgh";
    char buf[3] = {0};
    for (int r = 0; r != rows; ++r)
        for (int c = 0; c != columns; ++c)
        {
            buf[0] = syms[r];
            buf[1] = syms[c];
            m.setEntry(r, c, buf);
        }

    return m;
}

static VariableTable* generateVariableTable(SymbolicMatrix* m)
{
    static const char* syms = "abcdefgh";
    char buf[3] = {0};
    VariableTable* vt = new VariableTable;

    /*
     * These values were chosen such that
     *    det(m) = 0.0607754383633464
     * for 8x8
     */
    double value1 = 1;
    double value2 = 1;
    for (int r = 0; r != m->rows(); ++r)
        for (int c = 0; c != m->columns(); ++c)
        {
            buf[0] = syms[r];
            buf[1] = syms[c];

            if (r == c)
            {
                vt->set(buf, value1);
                value1 += (value1 * 0.02) + 0.01;
            }
            else
            {
                vt->set(buf, value2);
                value2 -= (value2 * 0.02) + 0.01;
            }
        }

    return vt;
}

static void substituteWithValues(SymbolicMatrix* m)
{
    Reference<VariableTable> vt = generateVariableTable(m);
    for (int r = 0; r != m->rows(); ++r)
        for (int c = 0; c != m->columns(); ++c)
        {
            m->entry(r, c)->insertSubstitutions(vt);
            m->entry(r, c)->optimise();
        }
}

static SymbolicMatrix generateMatrix(int rows, int columns)
{
    /*
     * These values were chosen such that
     *    det(m) = 0.0607754383633464
     */
    SymbolicMatrix m = generateSymMatrix(rows, columns);
    substituteWithValues(&m);
    return m;
}

TEST(NAME, construct_with_size)
{
    SymbolicMatrix m(4, 7);
    ASSERT_THAT(m.rows(), Eq(4));
    ASSERT_THAT(m.columns(), Eq(7));
}

TEST(NAME, resize)
{
    SymbolicMatrix m;
    m.resize(5, 2);
    ASSERT_THAT(m.rows(), Eq(5));
    ASSERT_THAT(m.columns(), Eq(2));
}

TEST(NAME, fill_with_zeros_works)
{
    SymbolicMatrix m(2, 2);
    m.fillZeros();
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(1, 1)->value(), DoubleEq(0));
}

TEST(NAME, fill_identity_works)
{
    SymbolicMatrix m(3, 3);
    m.fillIdentity();
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(1));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(2, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(1, 1)->value(), DoubleEq(1));
    ASSERT_THAT(m.entry(1, 2)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(2, 0)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(2, 1)->value(), DoubleEq(0));
    ASSERT_THAT(m.entry(2, 2)->value(), DoubleEq(1));
}

TEST(NAME, change_entry)
{
    SymbolicMatrix m(2, 2);
    m.setEntry(0, 1, Expression::make(3.4));
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(3.4));
    m.setEntry(0, 1, "5.5 + 9");
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(14.5));
}

TEST(NAME, set_entries_variadic)
{
    SymbolicMatrix m1(2, 2,
        1, 2,
        3, 4);
    ASSERT_THAT(m1.entry(0, 0)->value(), DoubleEq(1));
    ASSERT_THAT(m1.entry(0, 1)->value(), DoubleEq(2));
    ASSERT_THAT(m1.entry(1, 0)->value(), DoubleEq(3));
    ASSERT_THAT(m1.entry(1, 1)->value(), DoubleEq(4));

    SymbolicMatrix m2(2, 2,
        1, 2,
        3, 4);
    ASSERT_THAT(m2.entry(0, 0)->value(), DoubleEq(1));
    ASSERT_THAT(m2.entry(0, 1)->value(), DoubleEq(2));
    ASSERT_THAT(m2.entry(1, 0)->value(), DoubleEq(3));
    ASSERT_THAT(m2.entry(1, 1)->value(), DoubleEq(4));
}

TEST(NAME, add_2x3_with_2x3)
{
    SymbolicMatrix m = SymbolicMatrix(2, 3,
        1, 2, 3,
        4, 5, 6
    ).add(SymbolicMatrix(2, 3,
        6, 5, 4,
        3, 2, 1
    ));
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(3));
    for (int r = 0; r != 2; ++r)
        for (int c = 0; c != 3; ++c)
            ASSERT_THAT(m.entry(r, c)->value(), DoubleEq(7));
}

TEST(NAME, add_2x2_with_scalar)
{
    Reference<Expression> e = Expression::make(3.0);
    SymbolicMatrix m = SymbolicMatrix(2, 2, 1, 1, 1, 1).add(e);
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(2));
    for (int r = 0; r != 2; ++r)
        for (int c = 0; c != 2; ++c)
            ASSERT_THAT(m.entry(r, c)->value(), DoubleEq(4));
}

TEST(NAME, add_incompatible_matrices)
{
    Sequence s;
    StrictMock<MockTearListener> tears;
    EXPECT_CALL(tears, onTears(StrEq("Matrices must have the same size. Got 2x2 and 2x3")))
        .InSequence(s);
    EXPECT_CALL(tears, onTears(StrEq("Matrices must have the same size. Got 2x2 and 3x2")))
        .InSequence(s);
    SymbolicMatrix(2, 2, 0, 0, 0, 0).add(SymbolicMatrix(2, 3, 0, 0, 0, 0, 0, 0));
    SymbolicMatrix(2, 2, 0, 0, 0, 0).add(SymbolicMatrix(3, 2, 0, 0, 0, 0, 0, 0));
}

TEST(NAME, subtract_2x3_with_2x3)
{
    SymbolicMatrix m = SymbolicMatrix(2, 3,
        1, 2, 3,
        4, 5, 6
    ).sub(SymbolicMatrix(2, 3,
        2, 3, 4,
        5, 6, 7
    ));
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(3));
    for (int r = 0; r != 2; ++r)
        for (int c = 0; c != 3; ++c)
            ASSERT_THAT(m.entry(r, c)->value(), DoubleEq(-1));
}

TEST(NAME, subtract_2x2_with_scalar)
{
    Reference<Expression> e = Expression::make(3.0);
    SymbolicMatrix m = SymbolicMatrix(2, 2, 1, 1, 1, 1).sub(e);
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(2));
    for (int r = 0; r != 2; ++r)
        for (int c = 0; c != 2; ++c)
            ASSERT_THAT(m.entry(r, c)->value(), DoubleEq(-2));
}

TEST(NAME, subtract_incompatible_matrices)
{
    Sequence s;
    StrictMock<MockTearListener> tears;
    EXPECT_CALL(tears, onTears(StrEq("Matrices must have the same size. Got 2x2 and 2x3")))
        .InSequence(s);
    EXPECT_CALL(tears, onTears(StrEq("Matrices must have the same size. Got 2x2 and 3x2")))
        .InSequence(s);
    SymbolicMatrix(2, 2, 0, 0, 0, 0).sub(SymbolicMatrix(2, 3, 0, 0, 0, 0, 0, 0));
    SymbolicMatrix(2, 2, 0, 0, 0, 0).sub(SymbolicMatrix(3, 2, 0, 0, 0, 0, 0, 0));
}

TEST(NAME, multiply_1x3_with_3x1)
{
    SymbolicMatrix m = SymbolicMatrix(1, 3, 2, 3, 4)
        .mul(SymbolicMatrix(3, 1, 2, 3, 4));
    ASSERT_THAT(m.rows(), Eq(1));
    ASSERT_THAT(m.columns(), Eq(1));
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(2*2+3*3+4*4));
}

TEST(NAME, multiply_3x1_with_1x3)
{
    SymbolicMatrix m = SymbolicMatrix(3, 1, 2, 3, 4)
        .mul(SymbolicMatrix(1, 3, 2, 3, 4));
    ASSERT_THAT(m.rows(), Eq(3));
    ASSERT_THAT(m.columns(), Eq(3));
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(4));
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(6));
    ASSERT_THAT(m.entry(0, 2)->value(), DoubleEq(8));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(6));
    ASSERT_THAT(m.entry(1, 1)->value(), DoubleEq(9));
    ASSERT_THAT(m.entry(1, 2)->value(), DoubleEq(12));
    ASSERT_THAT(m.entry(2, 0)->value(), DoubleEq(8));
    ASSERT_THAT(m.entry(2, 1)->value(), DoubleEq(12));
    ASSERT_THAT(m.entry(2, 2)->value(), DoubleEq(16));
}

TEST(NAME, multiply_2x3_with_3x2)
{
    /*
     *         [7  8 ]
     * [1 2 3] [9  10]
     * [4 5 6] [11 12]
     */
    SymbolicMatrix m = SymbolicMatrix(2, 3,
        1, 2, 3,
        4, 5, 6
    ).mul(SymbolicMatrix(3, 2,
        7,  8,
        9,  10,
        11, 12
    ));
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(2));
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(7+18+33));
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(8+20+36));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(28+45+66));
    ASSERT_THAT(m.entry(1, 1)->value(), DoubleEq(32+50+72));
}

TEST(NAME, multiply_2x2_with_scalar)
{
    Reference<Expression> e = Expression::make(3.0);
    SymbolicMatrix m = SymbolicMatrix(2, 2, 4, 4, 4, 4).mul(e);
    ASSERT_THAT(m.rows(), Eq(2));
    ASSERT_THAT(m.columns(), Eq(2));
    for (int r = 0; r != 2; ++r)
        for (int c = 0; c != 2; ++c)
            ASSERT_THAT(m.entry(r, c)->value(), DoubleEq(12));
}

TEST(NAME, multiply_incompatible_matrices)
{
    Sequence s;
    StrictMock<MockTearListener> tears;
    EXPECT_CALL(tears, onTears(StrEq("Matrices have unequal column/rows. Got 2x2 and 3x2, need m*n and l*o with n=l")))
        .InSequence(s);
    EXPECT_CALL(tears, onTears(StrEq("Matrices have unequal column/rows. Got 2x3 and 2x2, need m*n and l*o with n=l")))
        .InSequence(s);
    SymbolicMatrix(2, 2, 0, 0, 0, 0).mul(SymbolicMatrix(3, 2, 0, 0, 0, 0, 0, 0));
    SymbolicMatrix(2, 3, 0, 0, 0, 0, 0, 0).mul(SymbolicMatrix(2, 2, 0, 0, 0, 0));
}

TEST(NAME, determinant_of_1x1)
{
    Reference<Expression> e = SymbolicMatrix(1, 1, 5).determinant();
    ASSERT_THAT(e->value(), DoubleEq(5));
}

TEST(NAME, determinant_of_2x2)
{
    Reference<Expression> e = SymbolicMatrix(2, 2,
        3, 8,
        4, 6
    ).determinant();
    ASSERT_THAT(e->value(), DoubleEq(-14));
}

TEST(NAME, determinant_of_3x3)
{
    Reference<Expression> e = SymbolicMatrix(3, 3,
        6,  1,  1,
        4, -2,  5,
        2,  8,  7
    ).determinant();
    ASSERT_THAT(e->value(), DoubleEq(-306));
}

TEST(NAME, determinant_of_8x8)
{
    SymbolicMatrix m = generateMatrix(8, 8);

    Reference<Expression> e = generateMatrix(8, 8).determinant();
    ASSERT_THAT(e->value(), DoubleNear(0.0607754383633464, std::numeric_limits<double>::epsilon()));
}

TEST(NAME, determinant_of_non_square_matrix_fails)
{
    StrictMock<MockTearListener> tears;
    EXPECT_CALL(tears, onTears(StrEq("Matrix has to be square. Got 2x3")));
    Reference<Expression>(SymbolicMatrix(2, 3, 0, 0, 0, 0, 0, 0).determinant());
}

TEST(NAME, inverse_of_1x1)
{
    SymbolicMatrix m = SymbolicMatrix(1, 1, 5).inverse();
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(1.0/5));
}

TEST(NAME, inverse_of_2x2)
{
    SymbolicMatrix m = SymbolicMatrix(2, 2,
        4, 3,
        3, 2
    ).inverse();
    ASSERT_THAT(m.entry(0, 0)->value(), DoubleEq(-2));
    ASSERT_THAT(m.entry(0, 1)->value(), DoubleEq(3));
    ASSERT_THAT(m.entry(1, 0)->value(), DoubleEq(3));
    ASSERT_THAT(m.entry(1, 1)->value(), DoubleEq(-4));
}

TEST(NAME, inverse_of_3x3)
{
    SymbolicMatrix m(3, 3,
        6,  1,  1,
        4, -2,  5,
        2,  8,  7
    );
    m = m.inverse().mul(m);
    for (int r = 0; r != 3; ++r)
        for (int c = 0; c != 3; ++c)
        {
            if (r == c)
                EXPECT_THAT(m.entry(r, c)->value(), DoubleEq(1));
            else
                EXPECT_THAT(m.entry(r, c)->value(), DoubleNear(0, std::numeric_limits<double>::epsilon()));
        }
}

TEST(NAME, inverse_of_8x8)
{
    SymbolicMatrix m = generateMatrix(8, 8);

    m = m.inverse().mul(m);
    for (int r = 0; r != 8; ++r)
        for (int c = 0; c != 8; ++c)
        {
            const double absError = std::numeric_limits<double>::epsilon() * 20;
            if (r == c)
                EXPECT_THAT(m.entry(r, c)->value(), DoubleNear(1, absError));
            else
                EXPECT_THAT(m.entry(r, c)->value(), DoubleNear(0, absError));
        }
}

TEST(NAME, inverse_accuracy_symbolic)
{
    Reference<Expression>(Expression::make("0"))->dump("inverse_accuracy_symbolic.dot");
    for (int i = 1; i != 6; ++i)
    {
        SymbolicMatrix m = generateSymMatrix(i, i);
        m = m.inverse().mul(m);
        //m.entry(0, 0)->dump("inverse_accuracy_symbolic.dot", true);

        int ops = 0;
        for (int r = 0; r != i; ++r)
            for (int c = 0; c != i; ++c)
                ops += m.entry(r, c)->size();
        std::cout << i << "x" << i << " operations: " << ops << std::endl;
/*
        substituteWithValues(&m);
        std::cout << m << std::endl;*/
    }
}
