#pragma once

#include "sfgsym/config.h"
#include "sfgsym/parsers/expression_parser.y.h"

C_BEGIN

struct sfgsym_expr;

struct sfgsym_expr_parser
{
    sfgsymscan_t scanner;
    sfgsympstate* parser;
    struct sfgsym_expr* result;
    int (*log)(const char* fmt, ...);
};

SFGSYM_PUBLIC_API int
sfgsym_expr_parser_init(struct sfgsym_expr_parser* driver);

SFGSYM_PUBLIC_API void
sfgsym_expr_parser_deinit(struct sfgsym_expr_parser* driver);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_expr_from_string(struct sfgsym_expr_parser* driver, const char* str);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_expr_from_buffer(struct sfgsym_expr_parser* driver, const void* buffer, int len);

SFGSYM_PRIVATE_API void
sfgsym_expr_parser_give_result(struct sfgsym_expr_parser* driver, struct sfgsym_expr* expr);

C_END
