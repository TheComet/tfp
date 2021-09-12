#include "sfgsym/algo/mason.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
void
sfgsym_path_init(struct sfgsym_path* path)
{
    vector_init(&path->branches, sizeof(struct sfgsym_branch*));
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_deinit(struct sfgsym_path* path)
{
    vector_deinit(&path->branches);
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_list_init(struct sfgsym_path_list* path_list)
{
    vector_init(&path_list->paths, sizeof(struct sfgsym_path));
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_list_deinit(struct sfgsym_path_list* path_list)
{
    vector_deinit(&path_list->paths);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_graph_find_loops(struct sfgsym_path_list* loops,
                        const struct sfgsym_graph* graph)
{
    return -1;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_graph_find_forward_paths(struct sfgsym_path_list* paths,
                                const struct sfgsym_node* in,
                                const struct sfgsym_node* out)
{
    return -1;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_calculate_determinant(const struct sfgsym_path_list* loops)
{
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_calculate_path_subdeterminant(const struct sfgsym_path_list* path,
                                     const struct sfgsym_path_list* loops)
{
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_calculate_path_gain(const struct sfgsym_path* path)
{
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_mason_gain_rule(const struct sfgsym_path_list* paths, const struct sfgsym_path_list* loops)
{
    return NULL;
}
