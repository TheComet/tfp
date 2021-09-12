#pragma once

#include "sfgsym/config.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_graph;
struct sfgsym_node;

struct sfgsym_branch
{
    struct sfgsym_graph* owning_graph;

    struct sfgsym_node* source;
    struct sfgsym_node* dest;
    struct sfgsym_expr* weight;
};

SFGSYM_PUBLIC_API struct sfgsym_branch*
sfgsym_branch_create_with_weight(struct sfgsym_graph* graph,
                                 struct sfgsym_node* source,
                                 struct sfgsym_node* dest,
                                 struct sfgsym_expr* weight);

SFGSYM_PUBLIC_API struct sfgsym_branch*
sfgsym_branch_create(struct sfgsym_graph* graph,
                     struct sfgsym_node* source,
                     struct sfgsym_node* dest);

SFGSYM_PUBLIC_API void
sfgsym_branch_destroy(struct sfgsym_branch* branch);

SFGSYM_PUBLIC_API int
sfgsym_branch_set_weight(struct sfgsym_branch* branch, struct sfgsym_expr* weight);

C_END
