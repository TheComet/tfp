#include "sfgsym/sfg/node.h"
#include "cstructures/memory.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
struct sfgsym_node*
sfgsym_node_create(struct sfgsym_graph* graph, const char* name)
{
    struct sfgsym_node* node = MALLOC(sizeof *node);
    if (node == NULL)
        return NULL;

    node->name = MALLOC(strlen(name) + 1);
    if (node->name == NULL)
    {
        FREE(node);
        return NULL;
    }
    strcpy(node->name, name);

    vector_init(&node->incoming, sizeof(struct sfgsym_branch*));
    vector_init(&node->outgoing, sizeof(struct sfgsym_branch*));

    return node;
}

/* ------------------------------------------------------------------------- */
SFGSYM_PUBLIC_API void
sfgsym_node_destroy(struct sfgsym_node* node)
{
    vector_deinit(&node->outgoing);
    vector_deinit(&node->incoming);
    FREE(node->name);
    FREE(node);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_node_is_independent_variable(const struct sfgsym_node* node)
{
    return -1;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_node_is_dependent_variable(const struct sfgsym_node* node)
{
    return -1;
}

/* ------------------------------------------------------------------------- */
struct sfgsym_expr*
sfgsym_node_equation(const struct sfgsym_node* node)
{
    return NULL;
}
