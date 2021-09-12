#include "sfgsym/symbolic/expression.h"
#include "sfgsym/symbolic/subs_table.h"
#include <math.h>

/* ------------------------------------------------------------------------- */
sfgsym_real
sfgsym_expr_eval(const struct sfgsym_expr* expr,
                 const struct sfgsym_subs_table* st)
{
    switch (expr->type)
    {
        case SFGSYM_LITERAL  : return expr->data.literal;
        case SFGSYM_VARIABLE : return sfgsym_subs_table_eval_variable(st, expr->data.varname);
        case SFGSYM_INFINITY : return 0.0/0.0;  /* TODO implement limits */

        case SFGSYM_OP : break;  /* is handled outside of switch statement */
    }

    switch (sfgsym_expr_child_count(expr))
    {
        case 2 : return expr->data.op(  /* 2 is most likely */
                    sfgsym_expr_eval(expr->child[0], st),
                    sfgsym_expr_eval(expr->child[1], st));
        case 1 : return expr->data.op(
                    sfgsym_expr_eval(expr->child[0], st));
        case 3 : return expr->data.op(
                    sfgsym_expr_eval(expr->child[0], st),
                    sfgsym_expr_eval(expr->child[1], st),
                    sfgsym_expr_eval(expr->child[2], st));
        case 4 : return expr->data.op(
                    sfgsym_expr_eval(expr->child[0], st),
                    sfgsym_expr_eval(expr->child[1], st),
                    sfgsym_expr_eval(expr->child[2], st),
                    sfgsym_expr_eval(expr->child[3], st));
        case 5 : return expr->data.op(
                    sfgsym_expr_eval(expr->child[0], st),
                    sfgsym_expr_eval(expr->child[1], st),
                    sfgsym_expr_eval(expr->child[2], st),
                    sfgsym_expr_eval(expr->child[3], st),
                    sfgsym_expr_eval(expr->child[4], st));
        case 6 : return expr->data.op(
                    sfgsym_expr_eval(expr->child[0], st),
                    sfgsym_expr_eval(expr->child[1], st),
                    sfgsym_expr_eval(expr->child[2], st),
                    sfgsym_expr_eval(expr->child[3], st),
                    sfgsym_expr_eval(expr->child[4], st),
                    sfgsym_expr_eval(expr->child[5], st));
    }

    /* I don't see there ever being operators with more than 6 arguments */
    return 0.0 / 0.0;
}
