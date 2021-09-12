#pragma once

#include "sfgsym/config.h"
#include "cstructures/vector.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_graph;
struct sfgsym_node;

/*!
 * \brief A path in the graph. The path can be a loop, or it can be a forward
 * path.
 */
struct sfgsym_path
{
    struct cs_vector branches;  /* list of struct sfgsym_branch* */
};

SFGSYM_PUBLIC_API void
sfgsym_path_init(struct sfgsym_path* path);

SFGSYM_PUBLIC_API void
sfgsym_path_deinit(struct sfgsym_path* path);

struct sfgsym_path_list
{
    struct cs_vector paths;  /* list of struct sfgsym_path */
};

SFGSYM_PUBLIC_API void
sfgsym_path_list_init(struct sfgsym_path_list* path_list);

SFGSYM_PUBLIC_API void
sfgsym_path_list_deinit(struct sfgsym_path_list* path_list);

SFGSYM_PUBLIC_API int
sfgsym_graph_find_loops(struct sfgsym_path_list* loops, const struct sfgsym_graph* graph);

SFGSYM_PUBLIC_API int
sfgsym_graph_find_forward_paths(struct sfgsym_path_list* paths, const struct sfgsym_node* in, const struct sfgsym_node* out);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_calculate_determinant(const struct sfgsym_path_list* loops);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_calculate_path_subdeterminant(const struct sfgsym_path_list* path, const struct sfgsym_path_list* loops);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_calculate_path_gain(const struct sfgsym_path* path);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_mason_gain_rule(const struct sfgsym_path_list* paths, const struct sfgsym_path_list* loops);

C_END
