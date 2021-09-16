#include "sfgsym/algo/mason.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/sfg/branch.h"
#include "sfgsym/symbolic/expression.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_path_gain_expr(const struct sfgsym_path* path)
{
    struct sfgsym_expr* expr = NULL;
    PATH_FOR_EACH_BRANCH(path, branch)
        if (expr)
        {
            struct sfgsym_expr* product;
            struct sfgsym_expr* factor = sfgsym_expr_duplicate(branch->weight);
            if (factor == NULL)
                goto fail;

            product = sfgsym_expr_op_create(2, sfgsym_op_mul, expr, factor);
            if (product == NULL)
                goto fail;

            expr = product;
        }
        else
        {
            expr = sfgsym_expr_duplicate(branch->weight);
            if (expr == NULL)
                goto fail;
        }
    PATH_END_EACH

    return expr;

    fail : if (expr) sfgsym_expr_destroy_recurse(expr);
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_path_determinant_expr(const struct sfgsym_path_list* loops)
{
    return NULL;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_paths_are_touching(
        const struct sfgsym_path* p1,
        const struct sfgsym_path* p2)
{
    PATH_FOR_EACH_BRANCH(p1, b1)
        PATH_FOR_EACH_BRANCH(p2, b2)
            if (b1->source == b2->source || b1->dest == b2->dest)
                return 1;
        PATH_END_EACH
    PATH_END_EACH

    return 0;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_path_find_nontouching(
        struct sfgsym_path_list* nontouching,
        const struct sfgsym_path_list* candidates,
        const struct sfgsym_path* check_path)
{
    struct sfgsym_path_list result;
    sfgsym_path_list_init(&result);

    PATH_LIST_FOR_EACH(candidates, path)
        struct sfgsym_path* new_path;
        if (sfgsym_paths_are_touching(path, check_path))
            continue;

        new_path = sfgsym_path_list_add_new_path(&result);
        if (new_path == NULL)
            goto fail;

        PATH_FOR_EACH_BRANCH(path, branch)
            if (sfgsym_path_add_branch(new_path, branch) != 0)
                goto fail;
        PATH_END_EACH

    PATH_LIST_END_EACH

    vector_swap(&result.paths, &nontouching->paths);
    sfgsym_path_list_deinit(&result);

    return 0;

    fail: sfgsym_path_list_deinit(&result);
    return -1;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_path_subdeterminant_expr(
        const struct sfgsym_path* path,
        const struct sfgsym_path_list* loops)
{
    struct sfgsym_expr* expr;
    struct sfgsym_path_list nontouching_loops;
    sfgsym_path_list_init(&nontouching_loops);

    if (sfgsym_path_find_nontouching(&nontouching_loops, loops, path) != 0)
        goto find_nontouching_failed;

    expr = sfgsym_path_determinant_expr(&nontouching_loops);
    sfgsym_path_list_deinit(&nontouching_loops);
    return expr;

    find_nontouching_failed : sfgsym_path_list_deinit(&nontouching_loops);
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_path_mason_expr(
        const struct sfgsym_path_list* paths,
        const struct sfgsym_path_list* loops)
{
    return NULL;
}
