#include "gmock/gmock.h"
#include "model/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Expression_optimiser

using namespace testing;
using namespace dpsfg;

TEST(NAME, simplify_constant_expressions)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(4.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make(3.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(10.0));
}

TEST(NAME, simplify_constant_expressions_with_variables)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->right()->value(), DoubleEq(5.0));
}

TEST(NAME, add_variable_to_zero)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(op::add,
                Expression::make(0.0),
                Expression::make(0.0)),
            Expression::make(0.0)),
        Expression::make("a"));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, add_zero_to_variable)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(op::add,
                Expression::make("a"),
                Expression::make(0.0)),
            Expression::make(0.0)),
        Expression::make(0.0));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, subtract_zero_from_variable)
{
    tfp::Reference<Expression> e = Expression::make(op::sub,
        Expression::make("a"),
        Expression::make(op::sub,
            Expression::make(0.0),
            Expression::make(op::sub,
                Expression::make(0.0),
                Expression::make(0.0))));
    e->optimise();
    EXPECT_THAT(e->left(), IsNull());
    EXPECT_THAT(e->right(), IsNull());
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, subtract_variable_from_zero)
{
    // 0 - (0 - (0 - a)) = -a
    tfp::Reference<Expression> e = Expression::make(op::sub,
        Expression::make(0.0),
        Expression::make(op::sub,
            Expression::make(0.0),
            Expression::make(op::sub,
                Expression::make(0.0),
                Expression::make("a"))));
    e->optimise();
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right(), NotNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, multiply_expression_by_one)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, multiply_one_by_expression)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make(1.0),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, divide_expression_by_one)
{
    tfp::Reference<Expression> e = Expression::make(op::div,
        Expression::make("a"),
        Expression::make(op::div,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->name(), StrEq("a"));
}

TEST(NAME, multiply_expression_by_negative_one)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::mul,
            Expression::make(1.0),
            Expression::make(-1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, multiply_negative_one_by_expression)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make(1.0),
        Expression::make(op::mul,
            Expression::make(-1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, divide_expression_by_negative_one)
{
    tfp::Reference<Expression> e = Expression::make(op::div,
        Expression::make("a"),
        Expression::make(op::div,
            Expression::make(1.0),
            Expression::make(-1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION1));
    ASSERT_THAT(e->op1(), Eq(op::negate));
    ASSERT_THAT(e->left(), IsNull());
    ASSERT_THAT(e->right()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(e->right()->name(), StrEq("a"));
}

TEST(NAME, raise_expression_to_power_of_one)
{
    tfp::Reference<Expression> e = Expression::make(op::pow,
        Expression::make("a"),
        Expression::make(op::pow,
            Expression::make(1.0),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->name(), StrEq("a"));
}
