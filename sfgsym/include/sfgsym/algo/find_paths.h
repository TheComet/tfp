#pragma once

#include "sfgsym/config.h"
#include "cstructures/vector.h"

C_BEGIN

struct sfgsym_graph;
struct sfgsym_node;
struct sfgsym_path_list;

SFGSYM_PUBLIC_API int
sfgsym_graph_find_forward_paths(
        struct sfgsym_path_list* paths,
        const struct sfgsym_node* in,
        const struct sfgsym_node* out);

SFGSYM_PUBLIC_API int
sfgsym_graph_find_loops(
        struct sfgsym_path_list* loops,
        const struct sfgsym_graph* graph);

C_END
