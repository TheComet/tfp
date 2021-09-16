#pragma once

#include "sfgsym/config.h"

C_BEGIN

struct sfgsym_expr;
struct sfgsym_graph;
struct sfgsym_node;
struct sfgsym_path;
struct sfgsym_path_list;

/*!
 * \brief Calculates a path's gain as an expression.
 * \param path The path.
 * \return Returns the expression of the path, or NULL if an error occurred.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_path_gain_expr(const struct sfgsym_path* path);

/*!
 * \brief Calculates the graph's determinant from all loops. The determinant
 * is defined in Mason's Gain Rule as:
 *
 *    1
 *    - product of all loop gains
 *    + sum of products of all possible two non-touching loop gains
 *    - sum of products of all possible three non-touching loop gains
 *    + sum of products of all possible four non-touching loop gains
 *    - ...
 *
 * \param loops A list of paths that describe loops in the graph. This can be
 * obtained with the function \see sfgsym_algo_find_loops()
 * \return Returns an expression for the determinant. If an error occurs, NULL
 * is returned.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_path_determinant_expr(const struct sfgsym_path_list* loops);

/*!
 * \brief Checks if two paths share nodes or not.
 * \param p1 First path.
 * \param p2 Second path.
 * \return Returns non-zero if any node in p1 is shared with p2. Zero otherwise.
 */
SFGSYM_PUBLIC_API int
sfgsym_paths_are_touching(
        const struct sfgsym_path* p1,
        const struct sfgsym_path* p2);

/*!
 * \brief Returns a list of paths that do not share any nodes with the "check
 * path".
 * \param nontouching Result is written to this parameter.
 * \param candidates The input set of paths to check.
 * \param check_path The path to check each candidate against.
 * \return Returns 0 if successful. Non-zero if otherwise.
 */
SFGSYM_PUBLIC_API int
sfgsym_path_find_nontouching(
        struct sfgsym_path_list* nontouching,
        const struct sfgsym_path_list* candidates,
        const struct sfgsym_path* check_path);

/*!
 * \brief Calculates the subdeterminant of a forward path. The subdeterminant
 * is calculated the same way as the graph determinant, excpet only loops that
 * do not touch the specified forward path are considered.
 * \note This is a convenience function combining \see sfgsym_path_find_nontouching()
 * and \see sfgsym_path_determinant_expr().
 * \param path The forward path.
 * \param loops All loops in the graph.
 * \return Returns the subdeterminant as an expression, or NULL if an error
 * occurred.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_path_subdeterminant_expr(
        const struct sfgsym_path* path,
        const struct sfgsym_path_list* loops);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_path_mason_expr(
        const struct sfgsym_path_list* paths,
        const struct sfgsym_path_list* loops);

C_END
