#include "sfgsym/parsers/expression_parser_post.h"
#include "sfgsym/symbolic/expression.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_parser_collapse_lists(
        struct sfgsym_expr_parser* driver,
        struct sfgsym_expr* expr)
{
    int i, child_count = sfgsym_expr_child_count(expr);
    for (i = 0; i != child_count; ++i)
        sfgsym_expr_parser_collapse_lists(driver, expr->child[i]);

    if (child_count == 1 &&
        expr->data.op == NULL  /* lists will have op = NULL */)
    {
        struct sfgsym_expr* e = expr;
        expr = e->child[0];

        if (e->parent)
        {
            int j = sfgsym_expr_child_count(e->parent) - 1;
            for (; j >= 0; --j)
                if (e->parent->child[j] == e)
                    break;
            e->parent->child[j] = e->child[0];
        }
        e->child[0]->parent = e->parent;
        e->child[0] = NULL;
        sfgsym_expr_destroy_recurse(e);
    }

    return expr;
}
