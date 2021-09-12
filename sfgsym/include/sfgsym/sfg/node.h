#pragma once

#include "sfgsym/config.h"
#include "cstructures/vector.h"

C_BEGIN

struct sfgsym_graph;
struct sfgsym_branch;

struct sfgsym_node
{
    struct sfgsym_graph* owning_graph;

    struct cs_vector outgoing;  /* list of struct sfgsym_branch */
    struct cs_vector incoming;  /* list of struct sfgsym_branch */
    char* name;
};

SFGSYM_PUBLIC_API struct sfgsym_node*
sfgsym_node_create(struct sfgsym_graph* graph, const char* name);

SFGSYM_PUBLIC_API void
sfgsym_node_destroy(struct sfgsym_node* node);

/*!
 * \brief Checks if this node represents an independent variable in the graph.
 * This is true if the node only has outgoing branches, and false if the node
 * has any incoming branches.
 * \param node The node to check.
 * \return Non-zero if the node is an independent variable, otherwise 0.
 */
SFGSYM_PUBLIC_API int
sfgsym_node_is_independent_variable(const struct sfgsym_node* node);

/*!
 * \brief Checks if this node represents a dependent variable in the graph.
 * This is true if the node has any incoming branches, and false if the node
 * only has outgoing branches.
 * \param node The node to check.
 * \return Non-zero if the node is a dependent variable, otherwise 0.
 */
SFGSYM_PUBLIC_API int
sfgsym_node_is_dependent_variable(const struct sfgsym_node* node);

/*!
 * \brief Returns the equivalent equation represented by this node. For example,
 *
 *     x  a  z
 *     o---->o
 *          ^
 *         / -b
 *        /
 *       o
 *       y
 *
 * sfgsym_node_equation(z) would return the equation "z=x*a - b*y". By gathering
 * the equation of each node in the graph, you can convert the graph into its
 * equivalent system of equations.
 *
 * \note The node has to be a dependent variable for a valid equation to exist.
 * This can be checked with the function sfgsym_node_is_dependent_variable().
 *
 * \param node The node to create the equation for.
 * \return Returns the equation, or NULL if an error occurs. NULL can be
 * returned if the node is not a dependent variable, or if any of nodes
 * with incoming branches have empty names.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_node_equation(const struct sfgsym_node* node);

C_END
