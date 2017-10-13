#include "gmock/gmock.h"
#include "../model/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Expression_manipulator

using namespace testing;
using namespace dpsfg;

TEST(NAME, eliminate_divisions)
{
    tfp::Reference<Expression> e = Expression::parse("a/s");
    e->eliminateDivisionsAndSubtractions();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));
}

TEST(NAME, eliminate_divisions_with_constant_power)
{
    tfp::Reference<Expression> e = Expression::parse("a/s^2");
    e->eliminateDivisionsAndSubtractions();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));
}

TEST(NAME, eliminate_divisions_with_variable_power)
{
    tfp::Reference<Expression> e = Expression::parse("a/s^x");
    e->eliminateDivisionsAndSubtractions();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op1(), Eq(op::negate));
    ASSERT_THAT(e->right()->right()->right()->name(), StrEq("x"));
}
