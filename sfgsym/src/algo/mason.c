#include "sfgsym/algo/mason.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/sfg/branch.h"
#include "sfgsym/symbolic/expression.h"
#include "cstructures/memory.h"
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
    int i, j, k;
    struct sfgsym_expr* determinant;
    char* tt;
    int* lcomb;

    const int loop_count = sfgsym_path_list_count(loops);

    determinant = sfgsym_expr_literal_create(1.0);
    if (determinant == NULL)
        goto alloc_determinant_failed;

    /* Nothing to do if there are no loops */
    if (loop_count == 0)
        return determinant;

    /*
     * For k=0 and k=1 the expressions are trivial and can be computed manually
     * as follows: 1 - (L1 + L2 + ... + Li) where Li is the loop gain at index i.
     */
    for (i = 0; i != loop_count; ++i)
    {
        struct sfgsym_expr* gain;
        struct sfgsym_expr* result;

        gain = sfgsym_path_gain_expr(sfgsym_path_list_at(loops, i));
        if (gain == NULL)
            goto dup_loop_gain_failed;

        result = sfgsym_expr_op_create(2, sfgsym_op_sub, determinant, gain);
        if (result == NULL)
            goto create_op_failed;

        determinant = result;
        continue;

        create_op_failed     : sfgsym_expr_destroy_recurse(gain);
        dup_loop_gain_failed : goto calc_k1_failed;
    }

    /* If there are less than 2 loops, we are done */
    if (loop_count < 2)
        return determinant;

    /*
     * Check which combination of loops touch each other, and cache results
     * into a "touching table" or tt for short, because sfgsym_paths_are_touching()
     * can be fairly expensive to call over and over again.
     *
     * The table needs n(n-1)/2 bytes of memory instead of n^2 because we only
     * need unique combinations.
     */
    tt = MALLOC(sizeof(char) * loop_count*(loop_count-1)/2);
    if (tt == NULL)
        goto alloc_tt_failed;
    for (i = 0; i != loop_count-1; ++i)
        for (j = i+1; j != loop_count; ++j)
            tt[i+loop_count*(j-i-1)] = sfgsym_paths_are_touching(
                    sfgsym_path_list_at(loops, i),
                    sfgsym_path_list_at(loops, j));

    /*
     * Create array for holding the current combination of loops. In the worst
     * case all loops from 0 to N-1 will need to be referenced.
     */
    lcomb = MALLOC(sizeof(int) * loop_count);
    if (lcomb == NULL)
        goto alloc_counters_failed;

    /*
     * Start with going through all 2-loop combinations, then 3-loop combinations,
     * and so on until we reach k-loop combinations.
     */
    for (k = 2; k != loop_count+1; ++k)
    {
        int found_nontouching_loops = 0;

        /* We add or subtract the result of loop combinations depending on
         * k being odd or even */
        sfgsym_real (*combine_op)(sfgsym_real, sfgsym_real) =
                (k % 2 == 0 ? sfgsym_op_add : sfgsym_op_sub);

        /* The initial combination of loops is 0, 1, ... k */
        for (i = 0; i != k; ++i)
            lcomb[k-i-1] = i;

        while (1)
        {
            struct sfgsym_expr* combined_gain;
            struct sfgsym_expr* new_determinant;

            /* Check if any loops in the current combination touch each other */
            for (i = 0; i != k-1; ++i)
                for (j = i+1; j != k; ++j)
                {
                    const int l1 = lcomb[j];
                    const int l2 = lcomb[i];
                    if (tt[l1 + loop_count*(l2-l1-1)])
                        goto loops_touch;
                }

            /*
             * The current combination of loops do not touch each other, so
             * multiply their gains together.
             */
            combined_gain = sfgsym_path_gain_expr(sfgsym_path_list_at(loops, lcomb[0]));
            if (combined_gain == NULL)
                goto duplicate_loop1_gain_failed;
            for (i = 1; i != k; ++i)
            {
                struct sfgsym_expr* loop2_gain;
                struct sfgsym_expr* new_combined_gain;

                loop2_gain = sfgsym_path_gain_expr(sfgsym_path_list_at(loops, lcomb[i]));
                if (loop2_gain == NULL)
                    goto duplicate_loop2_gain_failed;

                new_combined_gain = sfgsym_expr_op_create(2, sfgsym_op_mul, combined_gain, loop2_gain);
                if (new_combined_gain == NULL)
                {
                    sfgsym_expr_destroy_recurse(loop2_gain);
                    goto duplicate_loop2_gain_failed;
                }

                combined_gain = new_combined_gain;
            }

            /* Now, add or subtract it to/from the final expression */
            new_determinant = sfgsym_expr_op_create(2, combine_op, determinant, combined_gain);
            if (new_determinant == NULL)
            {
                sfgsym_expr_destroy_recurse(combined_gain);
                goto calc_combined_gain_failed;
            }
            determinant = new_determinant;
            found_nontouching_loops = 1;

            /* Calculate next combination of loops */
            loops_touch:
            lcomb[0]++;
            for (i = 0; i != k; ++i)
            {
                if (lcomb[i] >= loop_count - i)
                {
                    if (i == k-1)
                        goto no_more_combinations;

                    lcomb[i+1]++;
                    for (j = i; j >= 0; --j)
                        lcomb[j] = lcomb[j+1]+1;
                    continue;
                }

                break;
            }

            continue;

            duplicate_loop2_gain_failed : sfgsym_expr_destroy_recurse(combined_gain);
            duplicate_loop1_gain_failed : goto calc_combined_gain_failed;
        } no_more_combinations:;

        /*
         * Can exit early if for the current k no non-touching loops were found,
         * because that necessarily means that for larger k, there won't be
         * any non-touching loops either.
         */
        if (!found_nontouching_loops)
            break;
    }

    FREE(lcomb);
    FREE(tt);

    return determinant;

    calc_combined_gain_failed :
    alloc_counters_failed     : FREE(tt);
    alloc_tt_failed           :
    calc_k1_failed            : sfgsym_expr_destroy_recurse(determinant);
    alloc_determinant_failed  : return NULL;
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
        goto calc_final_result_failed;

    sfgsym_path_list_deinit(&nontouching_loops);

    return result;

    calc_final_result_failed : sfgsym_expr_destroy_recurse(denominator);
    calc_determinant_failed  : sfgsym_expr_destroy_recurse(numerator);
    calc_numerator_failed    : sfgsym_path_list_deinit(&nontouching_loops);
    return NULL;
}
