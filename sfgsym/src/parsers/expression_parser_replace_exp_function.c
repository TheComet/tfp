#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/parsers/expression_parser_post.h"
#include "sfgsym/symbolic/expression.h"
#include "cstructures/vector.h"
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
static int
find_symbols(struct cs_vector* symbols, const char* name, const struct sfgsym_expr* expr)
{
    int i, child_count = sfgsym_expr_child_count(expr);

    if (expr->type == SFGSYM_VARIABLE && strcmp(expr->data.varname, name) == 0)
        if (vector_push(symbols, &expr) != 0)
            return -1;

    for (i = 0; i != child_count; ++i)
        if (find_symbols(symbols, name, expr->child[i]) != 0)
            return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */
static struct sfgsym_expr*
find_arglist(struct sfgsym_expr* e)
{
    int i, child_count = sfgsym_expr_child_count(e);

    if (child_count == 1 && e->data.op == NULL)
        return e;

    if (child_count != 2 || e->data.op != sfgsym_op_mul)
        return NULL;

    for (i = 0; i != child_count; ++i)
    {
        struct sfgsym_expr* found = find_arglist(e->child[i]);
        if (found)
            return found;
    }

    return NULL;
}

/* ------------------------------------------------------------------------- */
static int
fixup_exp(struct sfgsym_expr_parser* driver, struct sfgsym_expr* exp)
{
    struct sfgsym_expr* arglist;
    struct sfgsym_expr* e;

    /*
     * Ensure that the parent of the exp symbol is a multiplication operator,
     * because an expression such as "exp(3)" will be parsed as "exp*(3)" where
     * "(3)" is a list containing one child, namely "3"
     */
    if (exp->parent == NULL ||
        sfgsym_expr_child_count(exp->parent) != 2 ||
        exp->parent->data.op != sfgsym_op_mul)
    {
        driver->log("eexp() expects 1 argument, but none were found\n");
        return -1;
    }

    /* To make things less complicated, ensure that exp symbol is child[0] */
    if (exp->parent->child[0] != exp)
    {
        struct sfgsym_expr* tmp = exp->parent->child[0];
        exp->parent->child[0] = exp->parent->child[1];
        exp->parent->child[1] = tmp;
        assert(exp->parent->child[0] == exp);
    }

    /* Travel up product chain (if any) and then recursively search for any
     * arglist with 1 argument */
    e = exp->parent;
    while (e->parent &&
           sfgsym_expr_child_count(e->parent) == 2 &&
           e->parent->data.op == sfgsym_op_mul)
    {
        e = e->parent;
    }

    arglist = find_arglist(e);
    if (arglist == NULL)
    {
        driver->log("eexp() expects 1 argument\n");
        return -1;
    }
    assert(sfgsym_expr_child_count(arglist) == 1);

    /* Ensure arglist is child[1] to make things easier */
    if (arglist->parent->child[1] != arglist)
    {
        struct sfgsym_expr* tmp = arglist->parent->child[0];
        arglist->parent->child[0] = arglist->parent->child[1];
        arglist->parent->child[1] = tmp;
        assert(arglist->parent->child[1] != arglist);
    }

    /*
     * Swapping the found arglist with the current sibling of the exp node
     * puts it in the correct place in the tree
     */
    {
        struct sfgsym_expr* tmp;
        struct sfgsym_expr* a = exp->parent;
        struct sfgsym_expr* b = arglist->parent;

        tmp = a->child[1];
        a->child[1] = b->child[1];
        b->child[1] = tmp;

        a->child[1]->parent = a;
        b->child[1]->parent = b;
    }

    /* And now, collapse arglist */
    exp->parent->child[1] = arglist->child[0];
    arglist->child[0]->parent = exp->parent;

    /* Arglist can be destroyed (make sure to NULL child to avoid double free) */
    arglist->child[0] = NULL;
    sfgsym_expr_destroy_recurse(arglist);

    /* Change op to pow */
    exp->parent->data.op = sfgsym_op_pow;

    /* Change the "exp" function into the constant "e" */
    exp->data.varname[1] = '\0';

    return 0;
}

/* ------------------------------------------------------------------------- */
static int
resolve_exp(struct sfgsym_expr_parser* driver, struct sfgsym_expr* expr, struct cs_vector* symbols)
{
    if (find_symbols(symbols, "exp", expr) != 0)
        return -1;

    VECTOR_FOR_EACH(symbols, struct sfgsym_expr*, symbol_expr)
        if (fixup_exp(driver, *symbol_expr) != 0)
            return -1;
    VECTOR_END_EACH

    return 0;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_parser_replace_exp_function(
        struct sfgsym_expr_parser* driver,
        struct sfgsym_expr* expr)
{
    struct cs_vector symbols;
    vector_init(&symbols, sizeof(struct sfgsym_expr*));

    if (resolve_exp(driver, expr, &symbols) != 0)
        goto fail;

    vector_deinit(&symbols);
    return expr;

    fail : vector_deinit(&symbols);
    return NULL;
}
