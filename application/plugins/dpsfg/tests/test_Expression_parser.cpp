#include "gmock/gmock.h"
#include "../model/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Expression_parser

using namespace testing;
using namespace dpsfg;

TEST(NAME, simple_test)
{
    tfp::Reference<Expression> e = Expression::parse("a*b+c");
    ASSERT_THAT(e, NotNull());
    EXPECT_THAT(e->parent(), IsNull());
    EXPECT_THAT(e->value(), StrEq(""));
    EXPECT_THAT(e->op(), Eq('+'));
    ASSERT_THAT(e->left(), NotNull());
    ASSERT_THAT(e->right(), NotNull());
    EXPECT_THAT(e->right()->value(), StrEq("c"));
    EXPECT_THAT(e->left()->parent(), Eq(e));
    EXPECT_THAT(e->right()->parent(), Eq(e));
    Expression* f = e->right();
    ASSERT_THAT(f->left(), NotNull());
    ASSERT_THAT(f->right(), NotNull());
    EXPECT_THAT(f->op(), Eq('*'));
    EXPECT_THAT(f->value(), StrEq(""));
    EXPECT_THAT(f->left()->value(), StrEq("a"));
    EXPECT_THAT(f->right()->value(), StrEq("b"));
    EXPECT_THAT(f->left()->parent(), Eq(f));
    EXPECT_THAT(f->right()->parent(), Eq(f));
}

TEST(NAME, mul_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b*c");
    EXPECT_THAT(e->op(), Eq('+'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('*'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, power_before_mul)
{
    tfp::Reference<Expression> e = Expression::parse("a*b^c");
    EXPECT_THAT(e->op(), Eq('*'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('^'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, power_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b^c");
    EXPECT_THAT(e->op(), Eq('+'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('^'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, mul_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b*c");
    EXPECT_THAT(e->op(), Eq('-'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('*'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, power_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b^c");
    EXPECT_THAT(e->op(), Eq('-'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('^'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, div_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b/c");
    EXPECT_THAT(e->op(), Eq('+'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('/'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, power_before_div)
{
    tfp::Reference<Expression> e = Expression::parse("a/b^c");
    EXPECT_THAT(e->op(), Eq('/'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('^'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, div_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b/c");
    EXPECT_THAT(e->op(), Eq('-'));
    EXPECT_THAT(e->left()->value(), StrEq("a"));
    EXPECT_THAT(e->right()->op(), Eq('/'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->value(), StrEq("c"));
}

TEST(NAME, parenthesis_test)
{
    tfp::Reference<Expression> e = Expression::parse("(a+b)*c");
    EXPECT_THAT(e->op(), Eq('*'));
    EXPECT_THAT(e->right()->value(), StrEq("c"));
    EXPECT_THAT(e->left()->op(), Eq('+'));
    EXPECT_THAT(e->left()->left()->value(), StrEq("a"));
    EXPECT_THAT(e->left()->right()->value(), StrEq("b"));
}

TEST(NAME, more_complicated_test)
{
    tfp::Reference<Expression> e = Expression::parse("(a1+aa2*s^2)/(1+s/(x*23))");
    EXPECT_THAT(e->op(), Eq('/'));
    EXPECT_THAT(e->left()->op(), Eq('+'));
    EXPECT_THAT(e->left()->left()->value(), StrEq("a1"));
    EXPECT_THAT(e->left()->right()->op(), Eq('*'));
    EXPECT_THAT(e->left()->right()->left()->value(), StrEq("aa2"));
    EXPECT_THAT(e->left()->right()->right()->op(), Eq('^'));
    EXPECT_THAT(e->left()->right()->right()->left()->value(), StrEq("s"));
    EXPECT_THAT(e->left()->right()->right()->right()->value(), StrEq("2"));
    EXPECT_THAT(e->right()->op(), Eq('+'));
    EXPECT_THAT(e->right()->left()->value(), StrEq("1"));
    EXPECT_THAT(e->right()->right()->op(), Eq('/'));
    EXPECT_THAT(e->right()->right()->left()->value(), StrEq("s"));
    EXPECT_THAT(e->right()->right()->right()->op(), Eq('*'));
    EXPECT_THAT(e->right()->right()->right()->left()->value(), StrEq("x"));
    EXPECT_THAT(e->right()->right()->right()->right()->value(), StrEq("23"));
}
