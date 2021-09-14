#pragma once

#include "sfgsym/config.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_graph;
struct sfgsym_node;
struct sfgsym_path;
struct sfgsym_path_list;

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_algo_mason_determinant(const struct sfgsym_path_list* loops);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_algo_mason_path_subdeterminant(
        const struct sfgsym_path_list* path,
        const struct sfgsym_path_list* loops);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_algo_mason_path_gain(const struct sfgsym_path* path);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_algo_mason_gain_rule(
        const struct sfgsym_path_list* paths,
        const struct sfgsym_path_list* loops);

C_END
