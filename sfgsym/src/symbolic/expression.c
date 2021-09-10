#include "sfgsym/symbolic/expression.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

sfgsym_real sfgsym_op_add(sfgsym_real a, sfgsym_real b) { return a+b; }
sfgsym_real sfgsym_op_sub(sfgsym_real a, sfgsym_real b) { return a-b; }
sfgsym_real sfgsym_op_neg(sfgsym_real a)                { return -a;  }
sfgsym_real sfgsym_op_mul(sfgsym_real a, sfgsym_real b) { return a*b; }
sfgsym_real sfgsym_op_div(sfgsym_real a, sfgsym_real b) { return a/b; }
sfgsym_real sfgsym_op_pow(sfgsym_real a, sfgsym_real b) { return pow(a, b); }
sfgsym_real sfgsym_op_exp(sfgsym_real a)                { return exp(a); }
sfgsym_real sfgsym_op_sqrt(sfgsym_real a)               { return sqrt(a); }

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* sfgsym_expr_literal_create(sfgsym_real value)
{
    struct sfgsym_expr* literal = malloc(sizeof *literal);
    if (literal == NULL)
        return NULL;

    literal->type = SFGSYM_LITERAL;
    literal->data.literal = value;
    literal->parent = NULL;
    literal->child[0] = NULL;
    return literal;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* sfgsym_expr_variable_create(const char* name)
{
    struct sfgsym_expr* variable = malloc(sizeof *variable);
    if (variable == NULL)
        return NULL;

    variable->type = SFGSYM_VARIABLE;
    variable->data.varname = malloc(strlen(name) + 1);
    if (variable->data.varname == NULL)
    {
        free(variable);
        return NULL;
    }
    strcpy(variable->data.varname, name);
    variable->parent = NULL;
    variable->child[0] = NULL;
    return variable;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* sfgsym_expr_infinity_create(void)
{
    struct sfgsym_expr* infinity = malloc(sizeof *infinity);
    if (infinity == NULL)
        return NULL;

    infinity->type = SFGSYM_INFINITY;
    infinity->parent = NULL;
    infinity->child[0] = NULL;
    return infinity;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* sfgsym_expr_op_create(int argc, sfgsym_real (*op_func)(), ...)
{
    va_list ap;
    int i;

    struct sfgsym_expr* op = malloc(sizeof(struct sfgsym_expr) + sizeof(struct sfgsym_expr) * (argc-1));
    if (op == NULL)
        return NULL;

    op->type = SFGSYM_OP + (enum sfgsym_expr_type)(argc - 1);
    op->data.op = op_func;
    op->parent = NULL;

    va_start(ap, op_func);
    for (i = 0; i < argc; ++i)
    {
        struct sfgsym_expr* expr = va_arg(ap, struct sfgsym_expr*);
        op->child[i] = expr;
        expr->parent = op;
    }
    va_end(ap);

    return op;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_list_create(int argc)
{
    int i;
    struct sfgsym_expr* list = malloc(sizeof(struct sfgsym_expr) + sizeof(struct sfgsym_expr) * (argc-1));
    if (list == NULL)
        return NULL;

    list->type = (enum sfgsym_expr_type)(SFGSYM_OP + argc - 1);
    list->data.op = NULL;
    list->parent = NULL;

    for (i = 0; i < argc; ++i)
        list->child[i] = NULL;

    return list;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_list_realloc(struct sfgsym_expr* list, int argc)
{
    int i;
    int oldargc = sfgsym_expr_child_count(list);

    assert(list->type >= SFGSYM_OP);
    assert(list->data.op == NULL);

    for (i = argc; i < oldargc; ++i)
    {
        sfgsym_expr_destroy_recurse(list->child[i]);
    }

    if (argc > oldargc)
        list = realloc(list, sizeof(struct sfgsym_expr) + sizeof(struct sfgsym_expr) * (argc-1));
    if (list == NULL)
        return NULL;

    for (i = 0; i < argc; ++i)
    {
        if (i < oldargc && list->child[i])
            list->child[i]->parent = list;
        else
            list->child[i] = NULL;
    }

    list->type = (enum sfgsym_expr_type)(SFGSYM_OP + argc - 1);
    return list;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_list_realloc_append(struct sfgsym_expr* list, struct sfgsym_expr* expr)
{
    int argc = sfgsym_expr_child_count(list);

    assert(list->type >= SFGSYM_OP);
    assert(list->data.op == NULL);

    list = sfgsym_expr_list_realloc(list, argc + 1);
    if (list == NULL)
        return NULL;

    list->child[argc] = expr;
    expr->parent = list;

    return list;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_list_set(struct sfgsym_expr* list, int idx, struct sfgsym_expr* child)
{
    assert(list->type >= SFGSYM_OP);
    assert(list->data.op == NULL);

    if (list->child[idx])
        sfgsym_expr_destroy_recurse(list->child[idx]);

    list->child[idx] = child;
    child->parent = list;
}

/* ------------------------------------------------------------------------- */
static void
sfgsym_expr_destroy_recurse_no_unlink(struct sfgsym_expr* expr)
{
    int i = sfgsym_expr_child_count(expr) - 1;
    for (; i >= 0; i--)
    {
        if (expr->child[i])
            sfgsym_expr_destroy_recurse_no_unlink(expr->child[i]);
    }

    if (expr->type == SFGSYM_VARIABLE)
        free(expr->data.varname);

    free(expr);
}
void
sfgsym_expr_destroy_recurse(struct sfgsym_expr* expr)
{

    sfgsym_expr_unlink_from_parent(expr);
    sfgsym_expr_destroy_recurse_no_unlink(expr);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_expr_child_count(const struct sfgsym_expr* expr)
{
    if (expr->type < SFGSYM_OP)
        return 0;

    return (int)expr->type - SFGSYM_OP + 1;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_unlink_from_parent(struct sfgsym_expr* expr)
{
    if (expr->parent)
    {
        int i = sfgsym_expr_child_count(expr->parent) - 1;
        for (; i >= 0; i--)
            if (expr->parent->child[i] == expr)
            {
                expr->parent->child[i] = NULL;
                expr->parent = NULL;
                break;
            }
    }
}
