#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/node.h"
#include "sfgsym/sfg/branch.h"
#include "cstructures/memory.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
struct sfgsym_graph*
sfgsym_graph_create(void)
{
    struct sfgsym_graph* graph = MALLOC(sizeof *graph);
    if (graph == NULL)
        return NULL;

    sfgsym_graph_init(graph);
    return graph;
}

/* ------------------------------------------------------------------------- */
void
sfgsym_graph_destroy(struct sfgsym_graph* graph)
{
    sfgsym_graph_deinit(graph);
    FREE(graph);
}

/* ------------------------------------------------------------------------- */
void
sfgsym_graph_init(struct sfgsym_graph* graph)
{
    vector_init(&graph->nodes, sizeof(struct sfgsym_node*));
    vector_init(&graph->branches, sizeof(struct sfgsym_node*));
}

/* ------------------------------------------------------------------------- */
void
sfgsym_graph_deinit(struct sfgsym_graph* graph)
{
    GRAPH_FOR_EACH_NODE(graph, node)
        sfgsym_node_destroy(node);
    GRAPH_END_EACH

    GRAPH_FOR_EACH_BRANCH(graph, branch)
        sfgsym_branch_destroy(branch);
    GRAPH_END_EACH

    vector_deinit(&graph->branches);
    vector_deinit(&graph->nodes);
}

/* ------------------------------------------------------------------------- */
struct sfgsym_node*
sfgsym_graph_create_node(struct sfgsym_graph* graph, const char* name)
{
    struct sfgsym_node* node = sfgsym_node_create(graph, name);
    if (node == NULL)
        return NULL;

    if (vector_push(&graph->nodes, &node) != 0)
    {
        sfgsym_node_destroy(node);
        return NULL;
    }

    return node;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_graph_destroy_node(struct sfgsym_graph* graph, struct sfgsym_node* node)
{
    unsigned i;
    for (i = 0; i != graph_node_count(graph); ++i)
        if (node == graph_node_at(graph, i))
        {
            vector_erase_index(&graph->nodes, i);
            sfgsym_node_destroy(node);
            return 0;
        }

    return -1;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_branch*
sfgsym_graph_connect_a_to_b(struct sfgsym_graph* graph,
                            struct sfgsym_node* a,
                            struct sfgsym_node* b,
                            struct sfgsym_expr* weight)
{
    struct sfgsym_branch* branch = sfgsym_branch_create_with_weight(graph, a, b, weight);
    if (branch == NULL)
        return NULL;

    if (vector_push(&graph->branches, &branch) != 0)
    {
        sfgsym_branch_destroy(branch);
        return NULL;
    }

    return branch;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_graph_destroy_branch(struct sfgsym_graph* graph, struct sfgsym_branch* branch)
{
    unsigned i;
    for (i = 0; i != graph_branch_count(graph); ++i)
        if (branch == graph_branch_at(graph, i))
        {
            vector_erase_index(&graph->branches, i);
            sfgsym_branch_destroy(branch);
            return 0;
        }

    return 1;
}
