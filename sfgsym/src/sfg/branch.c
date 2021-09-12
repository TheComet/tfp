#include "sfgsym/sfg/branch.h"
#include "sfgsym/symbolic/expression.h"
#include "cstructures/memory.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
struct sfgsym_branch*
sfgsym_branch_create_with_weight(struct sfgsym_graph* graph,
                                 struct sfgsym_node* source,
                                 struct sfgsym_node* dest,
                                 struct sfgsym_expr* weight)
{
    struct sfgsym_branch* branch = MALLOC(sizeof *branch);
    if (branch == NULL)
        return NULL;

    branch->owning_graph = graph;
    branch->source = source;
    branch->dest = dest;
    branch->weight = weight;

    return branch;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_branch*
sfgsym_branch_create(struct sfgsym_graph* graph,
                     struct sfgsym_node* source,
                     struct sfgsym_node* dest)
{
    struct sfgsym_branch* branch = MALLOC(sizeof *branch);
    if (branch == NULL)
        return NULL;

     branch->owning_graph = graph;
     branch->source = source;
     branch->dest = dest;
     branch->weight = sfgsym_expr_literal_create(1.0);

     if (branch->weight == NULL)
     {
         FREE(branch);
         return NULL;
     }

     return branch;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_branch_destroy(struct sfgsym_branch* branch)
{
    sfgsym_expr_destroy_recurse(branch->weight);
    FREE(branch);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_branch_set_weight(struct sfgsym_branch* branch, struct sfgsym_expr* weight)
{
    sfgsym_expr_destroy_recurse(branch->weight);
    branch->weight = weight;
}
