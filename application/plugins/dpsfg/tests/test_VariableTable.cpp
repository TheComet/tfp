#include "gmock/gmock.h"
#include "model/Expression.hpp"
#include "model/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_VariableTable

using namespace testing;
using namespace dpsfg;

TEST(NAME, add_and_get_constant_value)
{
    VariableTable vt;
    vt.add("a", 4.0);
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.0));
}

TEST(NAME, adding_variable_expression_creates_new_entry)
{
    VariableTable vt;
    vt.add("a", "b");
    Expression* e = vt.get("b");
    EXPECT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(0));
}

TEST(NAME, get_value_of_multiple_expressions)
{
    VariableTable vt;
    vt.add("a", "1/b+c");
    vt.set("b", 4.0);
    vt.set("c", "2*d");
    vt.set("d", "2");
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.25));
}

TEST(NAME, cyclic_lookup)
{
    VariableTable vt;
    vt.add("a", "a");
    vt.add("b", "c");
    vt.set("c", "d");
    vt.set("d", "b");
    EXPECT_THROW(vt.valueOf("a"), std::runtime_error);
    EXPECT_THROW(vt.valueOf("c"), std::runtime_error);
}
