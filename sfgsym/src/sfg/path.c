#include "sfgsym/sfg/path.h"
#include <stddef.h>

/* ------------------------------------------------------------------------- */
void
sfgsym_path_init(struct sfgsym_path* path)
{
    vector_init(&path->branches, sizeof(struct sfgsym_branch*));
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_deinit(struct sfgsym_path* path)
{
    vector_deinit(&path->branches);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_path_add_branch(struct sfgsym_path* path, struct sfgsym_branch* branch)
{
    return vector_push(&path->branches, &branch);
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_list_init(struct sfgsym_path_list* path_list)
{
    vector_init(&path_list->paths, sizeof(struct sfgsym_path));
}

/* ------------------------------------------------------------------------- */
void
sfgsym_path_list_deinit(struct sfgsym_path_list* path_list)
{
    VECTOR_FOR_EACH(&path_list->paths, struct sfgsym_path, path)
        sfgsym_path_deinit(path);
    VECTOR_END_EACH
    vector_deinit(&path_list->paths);
}

/* ------------------------------------------------------------------------- */
struct sfgsym_path*
sfgsym_path_list_add_new_path(struct sfgsym_path_list* path_list)
{
    struct sfgsym_path* path = vector_emplace(&path_list->paths);
    if (path == NULL)
        return NULL;

    sfgsym_path_init(path);
    return path;
}
