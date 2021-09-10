#include "gmock/gmock.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"

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
    sfgsym_expr* expr = parse_string("a+b+c");
    sfgsym_expr_destroy_recurse(expr);
}
