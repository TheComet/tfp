#include "sfgsym/algo/find_paths.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/branch.h"
#include "sfgsym/sfg/node.h"
#include "cstructures/hashmap.h"
#include "cstructures/vector.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
static int
find_paths_recurse(
        struct sfgsym_path_list* paths,
        const struct sfgsym_branch* branch,
        const struct sfgsym_node* out,
        struct cs_hashmap* visited,
        struct cs_hashmap* blocked,
        struct cs_vector* stack)
{
    if (hashmap_exists(blocked, &branch->dest))
        return 0;

    switch (hashmap_insert(visited, &branch->dest, NULL))
    {
        case HM_EXISTS : return 0;
        case HM_OOM    : return -1;
        case HM_OK     : break;
    }
    if (vector_push(stack, &branch) != 0)
        return -1;

    if (branch->dest == out)
    {
        struct sfgsym_path* path = sfgsym_path_list_add_new_path(paths);
        if (path == NULL)
            return -1;

        VECTOR_FOR_EACH(stack, struct sfgsym_branch*, path_branch)
            if (sfgsym_path_add_branch(path, *path_branch) != 0)
                return -1;
        VECTOR_END_EACH
    }
    else
    {
        NODE_FOR_EACH_OUTGOING(branch->dest, child_branch)
            if (find_paths_recurse(paths, child_branch, out, visited, blocked, stack) != 0)
                return -1;
        NODE_END_EACH
    }

    vector_pop(stack);
    hashmap_erase(visited, &branch->dest);

    return 0;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_algo_find_forward_paths(
        struct sfgsym_path_list* paths,
        const struct sfgsym_node* in,
        const struct sfgsym_node* out)
{
    struct cs_hashmap visited, blocked;
    struct cs_vector stack;

    if (hashmap_init_with_options(&visited, sizeof(struct sfgsym_node*), 0, 64, hash32_aligned_ptr) != HM_OK)
        goto init_visited_failed;
    if (hashmap_init_with_options(&blocked, sizeof(struct sfgsym_node*), 0, 64, hash32_aligned_ptr) != HM_OK)
        goto init_aligned_failed;
    vector_init(&stack, sizeof(struct sfgsym_branch*));

    NODE_FOR_EACH_OUTGOING(in, branch)
        if (find_paths_recurse(paths, branch, out, &visited, &blocked, &stack) != 0)
            goto find_paths_failed;
    NODE_END_EACH

    hashmap_deinit(&visited);
    hashmap_deinit(&blocked);
    vector_deinit(&stack);
    return 0;

    find_paths_failed   : vector_deinit(&stack);
                          hashmap_deinit(&blocked);
    init_aligned_failed : hashmap_deinit(&visited);
    init_visited_failed : return -1;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_algo_find_loops(
        struct sfgsym_path_list* loops,
        const struct sfgsym_graph* graph)
{
    struct cs_hashmap visited, blocked;
    struct cs_vector stack;

    if (hashmap_init_with_options(&visited, sizeof(struct sfgsym_node*), 0, 64, hash32_aligned_ptr) != HM_OK)
        goto init_visited_failed;
    if (hashmap_init_with_options(&blocked, sizeof(struct sfgsym_node*), 0, 64, hash32_aligned_ptr) != HM_OK)
        goto init_aligned_failed;
    vector_init(&stack, sizeof(struct sfgsym_branch*));

    GRAPH_FOR_EACH_NODE(graph, node)
        NODE_FOR_EACH_OUTGOING(node, branch)
            if (find_paths_recurse(loops, branch, node, &visited, &blocked, &stack) != 0)
                goto find_paths_failed;
        NODE_END_EACH

        if (hashmap_insert(&blocked, &node, NULL) != HM_OK)
            goto find_paths_failed;
    GRAPH_END_EACH

    hashmap_deinit(&visited);
    hashmap_deinit(&blocked);
    vector_deinit(&stack);

    return 0;

    find_paths_failed   : vector_deinit(&stack);
                          hashmap_deinit(&blocked);
    init_aligned_failed : hashmap_deinit(&visited);
    init_visited_failed : return -1;

}

