#include "sfgsym/symbolic/expression.h"
#include <math.h>
#include <stddef.h>

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_constants_fold(struct sfgsym_expr* expr)
{
    int i, child_count = sfgsym_expr_child_count(expr);
    for (i = 0; i != child_count; ++i)
        sfgsym_expr_constants_fold(expr->child[i]);

    /* child count might have changed */
    child_count = sfgsym_expr_child_count(expr);

    if (child_count > 0 && expr->data.op != NULL)
    {
        int constant_child_count = 0;
        for (i = 0; i != child_count; ++i)
            if (expr->child[i]->type == SFGSYM_LITERAL)
                constant_child_count++;

        if (constant_child_count == child_count)
        {
            if (expr->data.op == sfgsym_op_add)
                expr->data.literal = expr->child[0]->data.literal + expr->child[1]->data.literal;
            else if (expr->data.op == sfgsym_op_sub)
                expr->data.literal = expr->child[0]->data.literal - expr->child[1]->data.literal;
            else if (expr->data.op == sfgsym_op_mul)
                expr->data.literal = expr->child[0]->data.literal * expr->child[1]->data.literal;
            else if (expr->data.op == sfgsym_op_div)
                expr->data.literal = expr->child[0]->data.literal / expr->child[1]->data.literal;
            else if (expr->data.op == sfgsym_op_pow)
                expr->data.literal = pow(expr->child[0]->data.literal, expr->child[1]->data.literal);
            else
            {
                return;
            }

            expr->type = SFGSYM_LITERAL;
            sfgsym_expr_destroy_recurse(expr->child[0]);
            sfgsym_expr_destroy_recurse(expr->child[1]);
        }
    }
}
