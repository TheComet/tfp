#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME Expression_optimiser

using namespace testing;
using namespace tfp;

TEST(NAME, exponentiate_two_equal_product_operands)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make("a"));
    e->dump("exponentiate_two_equal_product_operands.dot");
    e->optimise();
    e->dump("exponentiate_two_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, exponentiate_three_equal_product_operands)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make("a")));
    e->dump("exponentiate_three_equal_product_operands.dot");
    e->optimise();
    e->dump("exponentiate_three_equal_product_operands.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(3.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, exponentiate_chain_of_product_operands_with_constant_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(5.0),
            Expression::make("a")));
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot");
    e->optimise();
    e->dump("exponentiate_chain_of_product_operands_with_constant_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->right()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->right()->value(), DoubleEq(5.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, combine_exponents_simple_test)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(op::pow,
            Expression::make("a"),
            Expression::make("x")),
        Expression::make(op::pow,
            Expression::make("a"),
            Expression::make("y")));
    e->dump("combine_exponents_simple_test.dot");
    e->optimise();
    e->dump("combine_exponents_simple_test.dot", true);
}

TEST(NAME, combine_exponents_recursive_multiplies)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(op::mul,
            Expression::make(op::pow,
                Expression::make("a"),
                Expression::make("x")),
            Expression::make("s")),
        Expression::make(op::mul,
            Expression::make(op::pow,
                Expression::make("a"),
                Expression::make("y")),
            Expression::make("t")));
    e->dump("combine_exponents_simple_test.dot");
    e->optimise();
    e->dump("combine_exponents_simple_test.dot", true);
}
