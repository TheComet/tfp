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
    return sfgsym_expr_literal_create(1);
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
    struct sfgsym_path_list nontouching_loops;
    struct sfgsym_expr* denominator;
    struct sfgsym_expr* result;
    struct sfgsym_expr* numerator = NULL;

    sfgsym_path_list_init(&nontouching_loops);
    PATH_LIST_FOR_EACH(paths, path)
        struct sfgsym_expr* subdeterminant_expr;
        struct sfgsym_expr* path_expr;
        struct sfgsym_expr* gain_expr;

        if (sfgsym_path_find_nontouching(&nontouching_loops, loops, path) != 0)
            goto find_nontouching_failed;

        subdeterminant_expr = sfgsym_path_determinant_expr(&nontouching_loops);
        if (subdeterminant_expr == NULL)
            goto calc_subdeterminant_failed;

        path_expr = sfgsym_path_gain_expr(path);
        if (path_expr == NULL)
            goto calc_gain_failed;

        gain_expr = sfgsym_expr_op_create(2, sfgsym_op_mul, path_expr, subdeterminant_expr);
        if (gain_expr == NULL)
            goto calc_gain_expr_failed;

        if (numerator == NULL)
        {
            numerator = gain_expr;
        }
        else
        {
            struct sfgsym_expr* new_result = sfgsym_expr_op_create(2, sfgsym_op_add, numerator, gain_expr);
            if (new_result == NULL)
                goto add_to_result_failed;
            numerator = new_result;
        }

        sfgsym_path_list_clear(&nontouching_loops);
        continue;

        add_to_result_failed       : sfgsym_expr_destroy_recurse(numerator);
        calc_gain_expr_failed      : sfgsym_expr_destroy_recurse(path_expr);
        calc_gain_failed           : sfgsym_expr_destroy_recurse(subdeterminant_expr);
        calc_subdeterminant_failed :
        find_nontouching_failed    : goto calc_numerator_failed;
    PATH_LIST_END_EACH

    denominator = sfgsym_path_determinant_expr(loops);
    if (loops == NULL)
        goto calc_determinant_failed;

    result = sfgsym_expr_op_create(2, sfgsym_op_div, numerator, denominator);
    if (result == NULL)
        goto calc_fina_result_failed;

    sfgsym_path_list_deinit(&nontouching_loops);

    return result;

    calc_fina_result_failed : sfgsym_expr_destroy_recurse(denominator);
    calc_determinant_failed : sfgsym_expr_destroy_recurse(numerator);
    calc_numerator_failed   : sfgsym_path_list_deinit(&nontouching_loops);
    return NULL;
}
