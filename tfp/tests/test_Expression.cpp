#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"

#define NAME Expression

using namespace testing;
using namespace tfp;

TEST(NAME, generate_variable_table)
{
    tfp::Reference<Expression> e = Expression::parse("a+b^c*d");
    tfp::Reference<VariableTable> vt = e->generateVariableTable();
    EXPECT_THAT(vt->valueOf("a"), DoubleEq(0));
    EXPECT_THAT(vt->valueOf("b"), DoubleEq(0));
    EXPECT_THAT(vt->valueOf("c"), DoubleEq(1));
    EXPECT_THAT(vt->valueOf("d"), DoubleEq(1));
}

TEST(NAME, evaluate_constant_expression)
{
    tfp::Reference<Expression> e = Expression::parse("(2+3*4)^2 + 4");
    EXPECT_THAT(e->evaluate(), DoubleEq(200));
}

TEST(NAME, evaluate_with_variables)
{
    tfp::Reference<Expression> e = Expression::parse("(a+3*c)^d + e");
    tfp::Reference<VariableTable> vt = new VariableTable;
    vt->add("a", 2);
    vt->add("c", 4);
    vt->add("d", 2);
    vt->add("e", 5);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(201));
}

TEST(NAME, evaluate_with_missing_variable_fails)
{
    tfp::Reference<Expression> e = Expression::parse("a");
    EXPECT_DEATH(e->evaluate(), ".*");
}
