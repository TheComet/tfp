#include "gmock/gmock.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/parsers/expression_parser.h"

#define NAME expression

using namespace testing;

TEST(NAME, evaluate_constant_expression)
{
    sfgsym_expr_parser parser;
    sfgsym_expr_parser_init(&parser);
    sfgsym_expr* e = sfgsym_expr_from_string(&parser, "(2+3*4)^2 + 4");
    sfgsym_expr_parser_deinit(&parser);

    EXPECT_THAT(sfgsym_expr_eval(e, nullptr), DoubleEq(200));
    sfgsym_expr_destroy_recurse(e);
}
