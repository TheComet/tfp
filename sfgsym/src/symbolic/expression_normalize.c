#include "sfgsym/symbolic/expression.h"

/* ------------------------------------------------------------------------- */
/*
 * a-(b-c) -> (c-b)+a
 *   -           +
 *  / \         / \
 * a   -       -   a
 *    / \     / \
 *   b   c   c   b
 */
static int
normalize_nested_subtraction(struct sfgsym_expr* e)
{
    struct sfgsym_expr* tmp;

    if (e->type != SFGSYM_OP + 1) return 0;
    if (e->data.op != sfgsym_op_sub) return 0;
    if (e->child[1]->type != SFGSYM_OP + 1) return 0;
    if (e->child[1]->data.op != sfgsym_op_sub) return 0;

    tmp = e->child[0];
    e->child[0] = e->child[1];
    e->child[1] = tmp;

    tmp = e->child[0]->child[0];
    e->child[0]->child[0] = e->child[0]->child[1];
    e->child[0]->child[1] = tmp;

    e->data.op = sfgsym_op_add;

    return 1;
}

/* ------------------------------------------------------------------------- */
/*
 * a+(b+c) -> (b+c)+a
 *   +           +
 *  / \         / \
 * a   +       +   a
 *    / \     / \
 *   b   c   b   c
 */
static int
normalize_nested_addition(struct sfgsym_expr* e)
{
    struct sfgsym_expr* tmp;

    if (e->type != SFGSYM_OP + 1) return 0;
    if (e->data.op != sfgsym_op_add) return 0;
    if (e->child[1]->type != SFGSYM_OP + 1) return 0;
    if (e->child[1]->data.op != sfgsym_op_add) return 0;

    tmp = e->child[0];
    e->child[0] = e->child[1];
    e->child[1] = tmp;

    return 1;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_expr_normalize_add_sub(struct sfgsym_expr* e)
{
    int i, result = 0;

    result += normalize_nested_addition(e);
    result += normalize_nested_subtraction(e);

    for (i = 0; i != sfgsym_expr_child_count(e); ++i)
        result += sfgsym_expr_normalize_add_sub(e->child[i]);
    return result;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_expr_normalize_mul_div(struct sfgsym_expr* e)
{
    int i, result = 0;

    for (i = 0; i != sfgsym_expr_child_count(e); ++i)
        result += sfgsym_expr_normalize_mul_div(e->child[i]);
    return result;
}
