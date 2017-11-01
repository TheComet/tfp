#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME VariableTable

using namespace testing;
using namespace tfp;

TEST(NAME, add_and_get_constant_value)
{
    VariableTable vt;
    vt.set("a", 4.0);
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.0));
}

TEST(NAME, get_value_of_multiple_expressions)
{
    VariableTable vt;
    vt.set("a", "1/b+c");
    vt.set("b", 4.0);
    vt.set("c", "2*d");
    vt.set("d", "2");
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.25));
}

TEST(NAME, cyclic_lookup)
{
    VariableTable vt;
    vt.set("a", "a");
    vt.set("b", "c");
    vt.set("c", "d");
    vt.set("d", "b");
    EXPECT_THROW(vt.valueOf("a"), std::runtime_error);
    EXPECT_THROW(vt.valueOf("c"), std::runtime_error);
}
