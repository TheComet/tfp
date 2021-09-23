#include "sfgsym/symbolic/expression.h"
#include <stddef.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
static int
expr_is_mul_div(const struct sfgsym_expr* e)
{
    if (e->type != SFGSYM_OP + 1) return 0;
    if (e->data.op == sfgsym_op_mul) return 1;
    if (e->data.op == sfgsym_op_div) return 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
static int
expr_is_add_sub(const struct sfgsym_expr* e)
{
    if (e->type != SFGSYM_OP + 1) return 0;
    if (e->data.op == sfgsym_op_add) return 1;
    if (e->data.op == sfgsym_op_sub) return 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
static int
match_and_collect(
        struct sfgsym_expr* src,
        struct sfgsym_expr* match,
        struct sfgsym_expr** new_src,
        struct sfgsym_expr** new_match)
{
    switch (expr_is_mul_div(src)*2 + expr_is_mul_div(match))
    {
        case 0 : {
            struct sfgsym_expr* parent = match->parent;
            struct sfgsym_expr* sibling;

            if (!sfgsym_expr_compare_trees(src, match))
                return 0;

            sibling = parent->child[0] == match ? parent->child[1] : parent->child[0];
            if (sibling == src)
            {
                struct sfgsym_expr* vars = src;
                struct sfgsym_expr* coeff = match;
                struct sfgsym_expr* term = match->parent;

                sfgsym_expr_morph_to_literal(coeff, src->parent->data.op == sfgsym_op_add ? 2.0 : 0.0);
                assert(term->type == SFGSYM_OP + 1);
                term->data.op = sfgsym_op_mul;

                term->child[0] = coeff;
                coeff->parent = term;

                term->child[1] = vars;
                vars->parent = term;

                *new_src = term;
                *new_match = sibling;
            }
            else
            {
                struct sfgsym_expr* vars = src;
                struct sfgsym_expr* coeff = match;
                struct sfgsym_expr* term = match->parent;
                struct sfgsym_expr* root = src->parent;
                assert(src->parent);
                assert(src->parent->child[1] == src);

                sfgsym_expr_collapse_into_parent_keep_parent(sibling);

                sfgsym_expr_morph_to_literal(coeff, src->parent->data.op == sfgsym_op_add ? 2.0 : 0.00);
                assert(term->type == SFGSYM_OP + 1);
                term->data.op = sfgsym_op_mul;

                root->child[1] = term;
                term->child[0] = coeff;
                term->child[1] = vars;

                term->parent = root;
                coeff->parent = term;
                vars->parent = term;

                *new_src = term;
                *new_match = sibling;
            }

            return 1;
        } break;

        case 1 : {
            struct sfgsym_expr* parent = match->parent;
            struct sfgsym_expr* sibling;

            if (!sfgsym_expr_compare_trees(src, match->child[1]))
                return 0;

            assert(match->child[0]->type == SFGSYM_LITERAL);
            match->child[0]->data.literal +=
                    (parent->data.op == sfgsym_op_sub && parent->child[1] == match ? -1.0 : 1.0);

            sfgsym_expr_swap(src, match);

            sibling = src->parent->child[0] == src ? src->parent->child[1] : src->parent->child[0];
            sfgsym_expr_collapse_into_parent(sibling);

            *new_src = match;
            *new_match = sibling;
            return 1;
        } break;

        case 2 : {
            struct sfgsym_expr* sibling;
            struct sfgsym_expr* parent = match->parent;

            if (!sfgsym_expr_compare_trees(src->child[1], match))
                return 0;

            assert(src->child[0]->type == SFGSYM_LITERAL);

            src->child[0]->data.literal +=
                    (parent->data.op == sfgsym_op_sub && parent->child[1] == match ? -1.0 : 1.0);

            sibling = parent->child[0] == match ? parent->child[1] : parent->child[0];
            sfgsym_expr_collapse_into_parent(sibling);

            *new_src = src;
            *new_match = sibling;
            return 1;
        } break;

        case 3 : {
            sfgsym_real sign;
            struct sfgsym_expr* sibling;
            struct sfgsym_expr* parent = match->parent;

            if (!sfgsym_expr_compare_trees(src->child[1], match->child[1]))
                return 0;

            assert(src->child[0]->type == SFGSYM_LITERAL);
            assert(match->child[0]->type == SFGSYM_LITERAL);

            sign = parent->data.op == sfgsym_op_sub && parent->child[1] == match ? -1.0 : 1.0;
            src->child[0]->data.literal += sign * match->child[0]->data.literal;

            sibling = parent->child[0] == match ? parent->child[1] : parent->child[0];
            sfgsym_expr_collapse_into_parent(sibling);

            *new_src = src;
            *new_match = sibling;
            return 1;
        } break;
    }

    return -1;
}

/* ------------------------------------------------------------------------- */
/*
 * Example of the most complex structure we have to handle:
 *
 *         +
 *        / \
 *       5   +
 *          / \
 *         +   *
 *        / \  |\
 *       +   * 2 a
 *      / \  |\
 *     *   * 2 b
 *    /|   |\
 *   3 b   3 c
 */
static struct sfgsym_expr*
collect_common_terms(struct sfgsym_expr* e)
{
    struct sfgsym_expr* next;
    struct sfgsym_expr* new_src;

    if (!expr_is_add_sub(e))
        return e;

    /*
     * Due to how additions/subtractions are normalized, there may be a single
     * constant term on the lhs which we can safely ignore
     */
    if (e->child[0]->type == SFGSYM_LITERAL)
        return collect_common_terms(e->child[1]);

    next = e->child[0];
    while (expr_is_add_sub(next))
    {
        switch (match_and_collect(e->child[1], next->child[1], &new_src, &next))
        {
            case  1 : if (new_src->parent != e) e = new_src; break;
            case  0 : next = next->child[0]; break;
            case -1 : return NULL;
        }
    }

    switch (match_and_collect(e->child[1], next, &new_src, &next))
    {
        case  1 : if (new_src->parent != e) e = new_src; break;
        case -1 : return NULL;
    }

    return e;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_expr_collect_common_terms(struct sfgsym_expr* e)
{
    int i;
    e = collect_common_terms(e);
    if (e == NULL)
        return NULL;

    for (i = 0; i != sfgsym_expr_child_count(e); ++i)
    {
        if (sfgsym_expr_collect_common_terms(e->child[i]) == NULL)
            return NULL;
    }

    return e;
}
