#include "gmock/gmock.h"
#include "../model/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Expression_parser

using namespace testing;
using namespace dpsfg;

TEST(NAME, simple_test)
{
    tfp::Reference<Expression> e = Expression::parse("a*b+3");
    ASSERT_THAT(e, NotNull());
    EXPECT_THAT(e->parent(), IsNull());
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::add));
    ASSERT_THAT(e->left(), NotNull());
    ASSERT_THAT(e->right(), NotNull());
    EXPECT_THAT(e->right()->symbol().type, Symbol::CONSTANT);
    EXPECT_THAT(e->right()->symbol().value, DoubleEq(3.0));
    EXPECT_THAT(e->left()->parent(), Eq(e));
    EXPECT_THAT(e->right()->parent(), Eq(e));
    Expression* f = e->left();
    ASSERT_THAT(f->left(), NotNull());
    ASSERT_THAT(f->right(), NotNull());
    EXPECT_THAT(f->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(f->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(f->left()->symbol().type, Eq(Symbol::VARIABLE));
    EXPECT_THAT(f->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(f->left()->symbol().type, Eq(Symbol::VARIABLE));
    EXPECT_THAT(f->right()->symbol().name, StrEq("b"));
    EXPECT_THAT(f->left()->parent(), Eq(f));
    EXPECT_THAT(f->right()->parent(), Eq(f));
}

TEST(NAME, mul_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b*c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::add));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, power_before_mul)
{
    tfp::Reference<Expression> e = Expression::parse("a*b^c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::pow));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, power_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b^c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::add));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::pow));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, mul_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b*c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::sub));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, power_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b^c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::sub));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::pow));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, div_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b/c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::add));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::div));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, power_before_div)
{
    tfp::Reference<Expression> e = Expression::parse("a/b^c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::div));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::pow));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, div_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b/c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::sub));
    EXPECT_THAT(e->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->right()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->right()->symbol().eval2, Eq(&Symbol::div));
    EXPECT_THAT(e->right()->left()->symbol().name, StrEq("b"));
    EXPECT_THAT(e->right()->right()->symbol().name, StrEq("c"));
}

TEST(NAME, parenthesis_test)
{
    tfp::Reference<Expression> e = Expression::parse("(a+b)*c");
    EXPECT_THAT(e->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(e->right()->symbol().name, StrEq("c"));
    EXPECT_THAT(e->left()->symbol().type, Eq(Symbol::FUNCTION2));
    EXPECT_THAT(e->left()->symbol().eval2, Eq(&Symbol::mul));
    EXPECT_THAT(e->left()->left()->symbol().name, StrEq("a"));
    EXPECT_THAT(e->left()->right()->symbol().name, StrEq("b"));
}
