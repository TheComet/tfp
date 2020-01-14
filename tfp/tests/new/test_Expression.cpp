#include "gmock/gmock.h"
#include "tfp/math/new/Expression.hpp"

#define NAME NewExpression

using namespace testing;
using namespace tfpnew;
using namespace tfp;

TEST(NAME, parse_simple)
{
    Reference<Expression> e = Expression::parse("a+b^c*d");
}
