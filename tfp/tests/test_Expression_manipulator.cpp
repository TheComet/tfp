#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME Expression_manipulator

using namespace testing;
using namespace tfp;

TEST(NAME, enforce_product_order)
{
    Reference<Expression> e = Expression::parse("a*b");
    e->enforceProductLHS("a");
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->name(), StrEq("b"));
    e->enforceProductLHS("b");
    EXPECT_THAT(e->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, eliminate_divisions)
{
    Reference<Expression> e = Expression::parse("a/s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_divisions_with_constant_exponent)
{
    Reference<Expression> e = Expression::parse("a/s^2");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));
}

TEST(NAME, eliminate_divisions_with_variable_exponent)
{
    Reference<Expression> e = Expression::parse("a/s^x");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("x"));
}

TEST(NAME, eliminate_subtractions)
{
    Reference<Expression> e = Expression::parse("a-s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_subtractions_with_constant_post_factor)
{
    Reference<Expression> e = Expression::parse("a-s*2");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));
}

TEST(NAME, eliminate_subtractions_with_variable_post_factor)
{
    Reference<Expression> e = Expression::parse("a-s*x");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_subtractions_with_constant_pre_factor)
{
    Reference<Expression> e = Expression::parse("a-2*s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->value(), DoubleEq(2));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_subtractions_with_variable_pre_factor)
{
    Reference<Expression> e = Expression::parse("a-x*s");
    EXPECT_THAT(e->eliminateDivisionsAndSubtractions("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, enforce_constant_exponent_on_missing_exponent)
{
    Reference<Expression> e = Expression::parse("1/(s+4)");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(true));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(1.0));
}

TEST(NAME, enforce_constant_exponent_on_existing_constant_exponent)
{
    Reference<Expression> e = Expression::parse("1/(s^3+4)");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(true));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(3.0));
}

TEST(NAME, enforce_constant_exponent_only_on_trees_that_have_variable)
{
    Reference<Expression> e = Expression::parse("(a+b)^c");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));

    e = Expression::parse("(a+s)^c");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(false));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    
    e = Expression::parse("(a+s)^2");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
}

TEST(NAME, enforce_constant_exponent_on_variable_exponent_fails)
{
    Reference<Expression> e = Expression::parse("1/(s^a+4)");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(false));
}

TEST(NAME, enforce_constant_exponent_on_expression_exponent_fails)
{
    Reference<Expression> e = Expression::parse("1/(s^(a+1)+4)");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(false));
}

TEST(NAME, enforce_constant_exponent_on_consant_exponent_expression)
{
    Reference<Expression> e = Expression::parse("1/(s^(4+1)+4)");
    ASSERT_THAT(e->enforceConstantExponent("s"), Eq(true));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(5.0));
}

TEST(NAME, expand_constant_exponents_4)
{
    Reference<Expression> e = Expression::parse("a^4");
    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    e->dump("expand_constant_exponents_4.dot");
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
}

TEST(NAME, expand_constant_exponents_1)
{
    Reference<Expression> e = Expression::parse("a^1");
    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, expand_constant_exponents_0)
{
    Reference<Expression> e = Expression::parse("a^0");
    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    ASSERT_THAT(e->value(), DoubleEq(1.0));
}

TEST(NAME, expand_constant_exponents_negative_1)
{
    Reference<Expression> e = Expression::parse("a^-1");
    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));
}

TEST(NAME, expand_constant_exponents_negative_4)
{
    Reference<Expression> e = Expression::parse("a^-4");
    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    e->dump("expand_constant_exponents_negative_4.dot");
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->right()->op2(), Eq(op::mul));
}

TEST(NAME, expand_scalar_into_sum)
{
    Reference<Expression> e = Expression::parse("a*(b+c)");
    ASSERT_THAT(e->expandProducts("b"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->right()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, expand_expression_into_sum)
{
    Reference<Expression> e = Expression::parse("(a+b)*(c+d)");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    vt->set("c", 13.0);
    vt->set("d", 17.0);
    double beforeResult = e->evaluate(vt);

    ASSERT_THAT(e->expandProducts("a"), Eq(true));
    e->dump("expand_expression_into_sum.dot");
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::add));

    // Make sure subtrees were actually duplicated
    ASSERT_THAT(e->left()->left(), Ne(e->right()->left()));
    EXPECT_THAT(e->evaluate(vt), DoubleEq(beforeResult));
}

TEST(NAME, expand_binomial_product)
{
    Reference<Expression> e = Expression::parse("(a+b)*(a+c)");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    vt->set("c", 13.0);
    double beforeResult = e->evaluate(vt);

    ASSERT_THAT(e->expandProducts("a"), Eq(true));
    e->dump("expand_binomial_product.dot");
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->evaluate(vt), DoubleEq(beforeResult));
}

TEST(NAME, expand_binomial_exponent)
{
    Reference<Expression> e = Expression::parse("(a+b)^2");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    double beforeResult = e->evaluate(vt);

    ASSERT_THAT(e->expandConstantExponentsIntoProducts("a"), Eq(true));
    ASSERT_THAT(e->expandProducts("a"), Eq(true));
    e->dump("expand_binomial_exponent.dot");
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->evaluate(vt), DoubleEq(beforeResult));
}

void beginDump(const char* filename);
void endDump();
TEST(NAME, compute_transfer_function_coefficient_expressions)
{
    Reference<Expression> e = Expression::parse("1/(1/s^3 - 4/(1+s)^2 - 8/(a+4))");
    /*
     * Solved on paper:
     *                      s^3 + 2s^4 + s^5
     *  G(s) = ---------------------------------------------
     *         1 + 2s + s^2 + (-4-x)s^3 + (-2x)s^4 + (-x)s^5
     * where:
     *     x = 8/(a+4)
     *
     * b0 = 0
     * b1 = 0
     * b2 = 0
     * b3 = 1
     * b4 = 2
     * b5 = 1
     * a0 = 1
     * a1 = 2
     * a2 = 1
     * a3 = -4-8/(a+4)
     * a4 = -16/(a+4)
     * a5 = -8/(a+4)
     */
    Expression::TransferFunctionCoefficients tfe = e->calculateTransferFunctionCoefficients("s");
    //e->dump("wtf.dot", true);

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
