#include "gmock/gmock.h"
#include "sfgsym/symbolic/Expression.hpp"

#define NAME NewExpression

using namespace testing;
using namespace sfgsym;

TEST(NAME, parse_simple)
{
    Reference<Expression> e = Expression::parse("a+b^c*d");
}
