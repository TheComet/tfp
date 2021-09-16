#pragma once

#include "sfgsym/config.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_expr_parser;

SFGSYM_PRIVATE_API struct sfgsym_expr*
sfgsym_expr_parser_replace_exp_function(
        struct sfgsym_expr_parser* driver,
        struct sfgsym_expr* expr);

SFGSYM_PRIVATE_API struct sfgsym_expr*
sfgsym_expr_parser_collapse_lists(
        struct sfgsym_expr_parser* driver,
        struct sfgsym_expr* expr);

C_END
