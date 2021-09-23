#include "sfgsym/symbolic/expression.h"
#include "cstructures/memory.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

sfgsym_real sfgsym_op_add(sfgsym_real a, sfgsym_real b) { return a+b; }
sfgsym_real sfgsym_op_sub(sfgsym_real a, sfgsym_real b) { return a-b; }
sfgsym_real sfgsym_op_mul(sfgsym_real a, sfgsym_real b) { return a*b; }
sfgsym_real sfgsym_op_div(sfgsym_real a, sfgsym_real b) { return a/b; }
sfgsym_real sfgsym_op_pow(sfgsym_real a, sfgsym_real b) { return pow(a, b); }

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* alloc_expr(int children)
{
    if (children < 1)
        children = 1;
    return MALLOC(sizeof(struct sfgsym_expr) + sizeof(struct sfgsym_expr*) * (children-1));
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr* sfgsym_expr_literal_create(sfgsym_real value)
{
    struct sfgsym_expr* literal = alloc_expr(0);
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
    struct sfgsym_expr* variable = alloc_expr(0);
    if (variable == NULL)
        return NULL;

    variable->type = SFGSYM_VARIABLE;
    variable->data.varname = MALLOC(strlen(name) + 1);
    if (variable->data.varname == NULL)
    {
        FREE(variable);
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
    struct sfgsym_expr* infinity = alloc_expr(0);
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

    struct sfgsym_expr* op = alloc_expr(argc);
    if (op == NULL)
        return NULL;

    op->type = (enum sfgsym_expr_type)(SFGSYM_OP + argc - 1);
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
    struct sfgsym_expr* list = alloc_expr(argc);
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
        list = REALLOC(list, sizeof(struct sfgsym_expr) + sizeof(struct sfgsym_expr*) * (argc-1));
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
sfgsym_expr_deinit_single(struct sfgsym_expr* expr)
{
    if (expr->type == SFGSYM_VARIABLE)
        FREE(expr->data.varname);
}
static void
sfgsym_expr_destroy_recurse_no_unlink(struct sfgsym_expr* expr);
static void
sfgsym_expr_deinit_recurse_no_unlink(struct sfgsym_expr* expr)
{
    int i = sfgsym_expr_child_count(expr) - 1;
    for (; i >= 0; i--)
    {
        if (expr->child[i])
            sfgsym_expr_destroy_recurse_no_unlink(expr->child[i]);
    }

    sfgsym_expr_deinit_single(expr);
}
static void
sfgsym_expr_destroy_recurse_no_unlink(struct sfgsym_expr* expr)
{
    sfgsym_expr_deinit_recurse_no_unlink(expr);

    FREE(expr);
}
void
sfgsym_expr_destroy_recurse(struct sfgsym_expr* expr)
{

    sfgsym_expr_unlink_from_parent(expr);
    sfgsym_expr_destroy_recurse_no_unlink(expr);
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_destroy_single(struct sfgsym_expr* expr)
{
    sfgsym_expr_deinit_single(expr);
    FREE(expr);
}

/* ------------------------------------------------------------------------- */
void sfgsym_expr_morph_to_literal(struct sfgsym_expr* expr, sfgsym_real value)
{
    sfgsym_expr_deinit_recurse_no_unlink(expr);
    expr->type = SFGSYM_LITERAL;
    expr->data.literal = value;
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

/* ------------------------------------------------------------------------- */
/*
 *     a                       a
 *    / \     collapse(c)     / \
 *   b   e        -->        c   e
 *  / \
 * d   c
 */
void
sfgsym_expr_collapse_into_parent(struct sfgsym_expr* c)
{
    int i;
    struct sfgsym_expr* a;
    struct sfgsym_expr* b;

    b = c->parent;
    assert(b);

    i = sfgsym_expr_child_count(b);
    for (; i >= 0; --i)
        if (b->child[i] == c)
        {
            b->child[i] = NULL;
            break;
        }

    a = b->parent;
    if (a)
    {
        i = sfgsym_expr_child_count(a) - 1;
        for (; i >= 0; --i)
            if (a->child[i] == b)
            {
                a->child[i] = c;
                break;
            }
    }
    c->parent = a;
    sfgsym_expr_destroy_recurse_no_unlink(b);
}

/* ------------------------------------------------------------------------- */
/*
 *     a                          a
 *    / \     collapse(c)        / \
 *   b   e        -->           c   e
 *  / \
 * d   c                  d  <--  (detatched node)
 */
void
sfgsym_expr_collapse_into_parent_keep_siblings(struct sfgsym_expr* c)
{
    int i;
    struct sfgsym_expr* a;
    struct sfgsym_expr* b;

    b = c->parent;
    assert(b);

    i = sfgsym_expr_child_count(b) - 1;
    for (; i >= 0; --i)
        b->child[i]->parent = NULL;

    a = b->parent;
    if (a)
    {
        i = sfgsym_expr_child_count(a) - 1;
        for (; i >= 0; --i)
            if (a->child[i] == b)
            {
                a->child[i] = c;
                break;
            }
    }
    c->parent = a;
    sfgsym_expr_destroy_single(b);
}

/* ------------------------------------------------------------------------- */
/*
 *     a                          a
 *    / \     collapse(c)        / \
 *   b   e        -->       b   c   e
 *  / \                    /
 * d   c                  d  <--  (detatched nodes)
 */
void
sfgsym_expr_collapse_into_parent_keep_parent(struct sfgsym_expr* c)
{
    int i;
    struct sfgsym_expr* a;
    struct sfgsym_expr* b;

    b = c->parent;
    assert(b);

    i = sfgsym_expr_child_count(b) - 1;
    for (; i >= 0; --i)
        if (b->child[i] == c)
        {
            b->child[i] = NULL;
            break;
        }

    a = b->parent;
    if (a)
    {
        i = sfgsym_expr_child_count(a) - 1;
        for (; i >= 0; --i)
            if (a->child[i] == b)
            {
                a->child[i] = c;
                break;
            }
    }
    c->parent = a;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_expr_swap(struct sfgsym_expr* a, struct sfgsym_expr* b)
{
    int ia, ib;
    struct sfgsym_expr* tmp;
    struct sfgsym_expr* pa = a->parent;
    struct sfgsym_expr* pb = b->parent;

    for (ia = sfgsym_expr_child_count(pa) - 1; ia >= 0; --ia)
        if (pa->child[ia] == a)
            break;
    for (ib = sfgsym_expr_child_count(pb) - 1; ia >= 0; --ib)
        if (pb->child[ib] == b)
            break;

    tmp = pa->child[ia];
    pa->child[ia] = pb->child[ib];
    pb->child[ib] = tmp;

    pa->child[ia]->parent = pa;
    pb->child[ib]->parent = pb;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_duplicate(const struct sfgsym_expr* expr)
{
    int i, child_count = sfgsym_expr_child_count(expr);
    struct sfgsym_expr* new_expr = alloc_expr(child_count);
    if (new_expr == NULL)
        return NULL;
    new_expr->type = expr->type;

    /*
     * Initial relationships to NULL, so in an error case we can safely call
     * sfgsym_expr_destroy_recurse()
     */
    new_expr->parent = NULL;
    for (i = 0; i != child_count; ++i)
        new_expr->child[i] = NULL;

    switch (expr->type)
    {
        case SFGSYM_LITERAL  : {
            new_expr->data.literal = expr->data.literal;
        } break;

        case SFGSYM_VARIABLE : {
            new_expr->data.varname = MALLOC(strlen(expr->data.varname) + 1);
            if (new_expr->data.varname == NULL)
            {
                FREE(new_expr);
                return NULL;
            }
            strcpy(new_expr->data.varname, expr->data.varname);
        } break;

        case SFGSYM_INFINITY : break;
        case SFGSYM_OP : break;
    }

    if (child_count > 0)
        new_expr->data.op = expr->data.op;

    for (i = 0; i != child_count; ++i)
    {
        new_expr->child[i] = sfgsym_expr_duplicate(expr->child[i]);
        if (new_expr->child[i] == NULL)
        {
            sfgsym_expr_destroy_recurse(new_expr);
            return NULL;
        }

        new_expr->child[i]->parent = new_expr;
    }

    return new_expr;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_expr_compare_trees(const struct sfgsym_expr* a, const struct sfgsym_expr* b)
{
    int i, cca, ccb;

    if (a->type != b->type)
        return 0;

    switch (a->type)
    {
        case SFGSYM_LITERAL: return a->data.literal == b->data.literal;
        case SFGSYM_VARIABLE: return strcmp(a->data.varname, b->data.varname) == 0;
        case SFGSYM_INFINITY:
        case SFGSYM_OP:
            break;
    }

    cca = sfgsym_expr_child_count(a) - 1;
    ccb = sfgsym_expr_child_count(b) - 1;
    if (cca != ccb)
        return 0;

    for (i = 0; i != cca; ++i)
        if (sfgsym_expr_compare_trees(a->child[0], b->child[0]))
            return 1;

    return 0;
}
