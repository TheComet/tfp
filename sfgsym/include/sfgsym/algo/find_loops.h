#pragma once

#include "sfgsym/config.h"

C_BEGIN

struct sfgsym_graph;
struct sfgsym_path_list;

SFGSYM_PUBLIC_API int
sfgsym_algo_find_loops(
        struct sfgsym_path_list* loops,
        const struct sfgsym_graph* graph);

C_END
