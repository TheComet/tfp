#pragma once

#include "sfgsym/config.h"
#include "cstructures/vector.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_node;
struct sfgsym_branch;

struct sfgsym_graph
{
    struct cs_vector nodes;     /* list of struct sfgsym_node* */
    struct cs_vector branches;  /* list of struct sfgsym_branch* */
};

SFGSYM_PUBLIC_API struct sfgsym_graph*
sfgsym_graph_create(void);

SFGSYM_PUBLIC_API void
sfgsym_graph_destroy(struct sfgsym_graph* graph);

SFGSYM_PUBLIC_API void
sfgsym_graph_init(struct sfgsym_graph* graph);

SFGSYM_PUBLIC_API void
sfgsym_graph_deinit(struct sfgsym_graph* graph);

/*!
 * \brief Create a new node and add it to the graph. The graph takes ownership
 * of the node, so it's fine to ignore the return value if you don't need it.
 * You will need it to destroy nodes again.
 * \param name The name to give the node. This is a user property and can be
 * anything. It should be unique (by convention) but there is nothing that
 * would break if two nodes were to share the same name. Typically, in electronics,
 * nodes will have names such as "V1", "V2", "I1", "I2", "Q1", "Q2"... where
 * the first letter denotes the physical unit of that node. There are graph
 * related functions that let you check if the physical units make sense or
 * not, and those rely on the node's name.
 * \return Non-owning pointer to the new node (graph takes ownership), or NULL
 * if allocation failed.
 */
SFGSYM_PUBLIC_API struct sfgsym_node*
sfgsym_graph_create_node(struct sfgsym_graph* graph, const char* name);

/*!
 * \brief Removes the node from the graph and destroys any incoming or outgoing
 * branches from that node.
 * \param node The node to remove and destroy.
 * \return Returns 0 if the node was found (and destroyed). Non-zero if the
 * node was not found.
 */
SFGSYM_PUBLIC_API int
sfgsym_graph_destroy_node(struct sfgsym_graph* graph, struct sfgsym_node* node);

/*!
 * \brief Creates a new directed branch between two nodes. The graph takes
 * ownership of the branch, so it's fine to ignore the returned value if you don't
 * need it. You will need it to destroy the branch again. The new branch will have
 * a default
 * \param a The outgoing node. Branch will be pointing away from this node.
 * \param b The incoming node. Branch will be pointing into this node.
 * \param weight The weight to give the branch. The branch will take ownership
 * of this parameter. If you specify NULL, then the weight will default to 1.0.
 * \return Non-owning pointer to the new branch (graph takes ownership), or NULL
 * if allocation fails.
 */
SFGSYM_PUBLIC_API struct sfgsym_branch*
sfgsym_graph_connect_a_to_b(
        struct sfgsym_graph* graph,
        struct sfgsym_node* a,
        struct sfgsym_node* b,
        struct sfgsym_expr* weight);

/*!
 * \brief Destroys a single branch connecting two nodes.
 * \param branch The branch to destroy.
 * \return Returns 0 if the branch was found (and destroyed). Non-zero if the
 * branch was not found.
 */
SFGSYM_PUBLIC_API int
sfgsym_graph_destroy_branch(struct sfgsym_graph* graph, struct sfgsym_branch* branch);

#define graph_node_count(graph) \
    (vector_count(&(graph)->nodes))

#define graph_node_at(graph, i) \
    (*(struct sfgsym_node**)vector_get_element(&(graph)->nodes, i))

#define graph_branch_count(graph) \
    (vector_count(&(graph)->branches))

#define graph_branch_at(graph, i) \
    (*(struct sfgsym_branch**)vector_get_element(&(graph)->branches, i))

#define GRAPH_FOR_EACH_NODE(graph, node_var) \
    VECTOR_FOR_EACH(&(graph)->nodes, struct sfgsym_node*, graph_##node_var) \
    struct sfgsym_node* node_var = *(graph_##node_var); {

#define GRAPH_FOR_EACH_BRANCH(graph, branch_var) \
    VECTOR_FOR_EACH(&(graph)->branches, struct sfgsym_branch*, graph_##branch_var) \
    struct sfgsym_branch* branch_var = *(graph_##branch_var); {

#define GRAPH_END_EACH \
    VECTOR_END_EACH }

C_END
