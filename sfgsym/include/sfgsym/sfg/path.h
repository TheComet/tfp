#pragma once

#include "sfgsym/config.h"
#include "cstructures/vector.h"

C_BEGIN

struct sfgsym_branch;

/*!
 * \brief A path in the graph. The path can be a loop, or it can be a forward
 * path.
 */
struct sfgsym_path
{
    struct cs_vector branches;  /* list of struct sfgsym_branch* */
};

SFGSYM_PUBLIC_API void
sfgsym_path_init(struct sfgsym_path* path);

SFGSYM_PUBLIC_API void
sfgsym_path_deinit(struct sfgsym_path* path);

SFGSYM_PUBLIC_API int
sfgsym_path_add_branch(struct sfgsym_path* path, struct sfgsym_branch* branch);

struct sfgsym_path_list
{
    struct cs_vector paths;  /* list of struct sfgsym_path */
};

SFGSYM_PUBLIC_API void
sfgsym_path_list_init(struct sfgsym_path_list* path_list);

SFGSYM_PUBLIC_API void
sfgsym_path_list_deinit(struct sfgsym_path_list* path_list);

SFGSYM_PUBLIC_API struct sfgsym_path*
sfgsym_path_list_add_new_path(struct sfgsym_path_list* path_list);

#define sfgsym_path_list_count(pl) \
    (vector_count(&(pl)->paths))

#define sfgsym_path_list_at(pl, i) \
    ((struct sfgsym_path*)vector_get_element(&(pl)->paths, i))

#define sfgsym_path_count(p) \
    (vector_count(&(p)->branches))

#define sfgsym_path_at(p, i) \
    (*(struct sfgsym_branch**)vector_get_element(&(p)->branches, i))

#define PATH_LIST_FOR_EACH(path_list, path_var) \
    VECTOR_FOR_EACH(&(path_list)->paths, struct sfgsym_path, path_var)

#define PATH_LIST_END_EACH \
    VECTOR_END_EACH

#define PATH_FOR_EACH_BRANCH(path, branch_var) \
    VECTOR_FOR_EACH(&(path)->branches, struct sfgsym_branch*, path_##branch_var) \
    struct sfgsym_branch* branch_var = *path_##branch_var; {

#define PATH_END_EACH \
    VECTOR_END_EACH }

C_END
