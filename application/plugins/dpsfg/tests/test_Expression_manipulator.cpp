#include "gmock/gmock.h"
#include "../model/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Expression_manipulator

using namespace testing;
using namespace dpsfg;

TEST(NAME, reorder_product)
{
    tfp::Reference<Expression> e = Expression::parse("a*b");
    e->reorderProducts("a");
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->name(), StrEq("b"));
    e->reorderProducts("b");
    EXPECT_THAT(e->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, eliminate_divisions)
{
    tfp::Reference<Expression> e = Expression::parse("a/s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_divisions_with_constant_exponent)
{
    tfp::Reference<Expression> e = Expression::parse("a/s^2");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));
}

TEST(NAME, eliminate_divisions_with_variable_exponent)
{
    tfp::Reference<Expression> e = Expression::parse("a/s^x");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op1(), Eq(op::negate));
    ASSERT_THAT(e->right()->right()->right()->name(), StrEq("x"));
}

TEST(NAME, eliminate_subtractions)
{
    tfp::Reference<Expression> e = Expression::parse("a-s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_subtractions_with_constant_post_factor)
{
    tfp::Reference<Expression> e = Expression::parse("a-s*2");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));
}

TEST(NAME, eliminate_subtractions_with_variable_post_factor)
{
    tfp::Reference<Expression> e = Expression::parse("a-s*x");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op1(), Eq(op::negate));
    ASSERT_THAT(e->right()->right()->right()->name(), StrEq("x"));
}

TEST(NAME, eliminate_subtractions_with_constant_pre_factor)
{
    tfp::Reference<Expression> e = Expression::parse("a-2*s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->value(), DoubleEq(2));
    ASSERT_THAT(e->right()->right()->op1(), Eq(op::negate));
    ASSERT_THAT(e->right()->right()->right()->name(), StrEq("s"));
}

TEST(NAME, eliminate_subtractions_with_variable_pre_factor)
{
    tfp::Reference<Expression> e = Expression::parse("a-x*s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->op1(), Eq(op::negate));
    ASSERT_THAT(e->right()->right()->right()->name(), StrEq("s"));
}
void beginDump(const char* filename);
void endDump();
TEST(NAME, compute_transfer_function_coefficient_expressions)
{
    tfp::Reference<Expression> e = Expression::parse("1/(1/s^2 - 4/(1+s) - 8/(a+4))");
    /*
     * Solved on paper:
     *                 (a+4)s^2 + (a+4)s^3
     *  G(s) = ---------------------------------
     *         (a+4) + (a+4)s + (8-4a)s^2 + 8s^3
     *
     * b0 = 0
     * b1 = 0
     * b2 = a+4
     * b3 = a+4
     * a0 = a+4
     * a1 = a+4
     * a2 = 8-4a
     * a3 = 8
     *
     */
    beginDump("wtf.dot");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    e->dump();
    endDump();

    Expression::TransferFunctionCoefficients tfe = e->calculateTransferFunctionCoefficients("s");
    ASSERT_THAT(tfe.numeratorCoefficients_.size(), Eq(4u));
    ASSERT_THAT(tfe.denominatorCoefficients_.size(), Eq(4u));
    Expression* b0 = tfe.numeratorCoefficients_[0];
    Expression* b1 = tfe.numeratorCoefficients_[1];
    Expression* b2 = tfe.numeratorCoefficients_[2];
    Expression* b3 = tfe.numeratorCoefficients_[3];
    Expression* a0 = tfe.denominatorCoefficients_[0];
    Expression* a1 = tfe.denominatorCoefficients_[1];
    Expression* a2 = tfe.denominatorCoefficients_[2];
    Expression* a3 = tfe.denominatorCoefficients_[3];
    EXPECT_THAT(b0->value(), DoubleEq(0));
    EXPECT_THAT(b1->value(), DoubleEq(0));
    EXPECT_THAT(b2->op2(), Eq(op::add));
    EXPECT_THAT(b3->op2(), Eq(op::add));
    EXPECT_THAT(a0->op2(), Eq(op::add));
    EXPECT_THAT(a1->op2(), Eq(op::add));
    EXPECT_THAT(a2->op2(), Eq(op::sub));
    EXPECT_THAT(a3->value(), DoubleEq(8));
}
