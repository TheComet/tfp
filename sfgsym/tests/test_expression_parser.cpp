#include "gmock/gmock.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/exporters/expr2dot.h"

#define NAME expression_parser

using namespace testing;

static sfgsym_expr* parse_string(const char* str)
{
    sfgsym_expr_parser parser;
    sfgsym_expr_parser_init(&parser);
    sfgsym_expr* expr = sfgsym_expr_from_string(&parser, str);
    sfgsym_expr_parser_deinit(&parser);
    return expr;
}

TEST(NAME, bla)
{
    sfgsym_expr* expr = parse_string("a+b*c^e(f+g, 2, 5)h");
    ASSERT_THAT(expr, NotNull());
    sfgsym_export_expr_dot_file(expr, "test.dot");
    sfgsym_expr_destroy_recurse(expr);
}
