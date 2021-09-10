#pragma once

#include "sfgsym/config.h"

C_BEGIN

enum sfgsym_expr_type
{
    SFGSYM_LITERAL,
    SFGSYM_VARIABLE,
    SFGSYM_INFINITY,
    SFGSYM_OP
};

struct sfgsym_expr
{
    enum sfgsym_expr_type type;

    union {
        char* varname;
        sfgsym_real literal;
        sfgsym_real (*op)();
    } data;

    struct sfgsym_expr* parent;
    struct sfgsym_expr* child[1];
};

SFGSYM_PUBLIC_API sfgsym_real sfgsym_op_add(sfgsym_real a, sfgsym_real b);
SFGSYM_PUBLIC_API sfgsym_real sfgsym_op_sub(sfgsym_real a, sfgsym_real b);
SFGSYM_PUBLIC_API sfgsym_real sfgsym_op_mul(sfgsym_real a, sfgsym_real b);
SFGSYM_PUBLIC_API sfgsym_real sfgsym_op_div(sfgsym_real a, sfgsym_real b);
SFGSYM_PUBLIC_API sfgsym_real sfgsym_op_pow(sfgsym_real a, sfgsym_real b);

SFGSYM_PUBLIC_API struct sfgsym_expr* sfgsym_expr_literal_create(sfgsym_real value);
SFGSYM_PUBLIC_API struct sfgsym_expr* sfgsym_expr_variable_create(const char* name);
SFGSYM_PUBLIC_API struct sfgsym_expr* sfgsym_expr_infinity_create(void);
SFGSYM_PUBLIC_API struct sfgsym_expr* sfgsym_expr_op_create(int argc, sfgsym_real (*op_func)(), ...);

/*!
 * \brief Creates a new list with children initialized to NULL.
 * \param argc Number of children to allocate.
 * \return Returns the new list, or NULL if allocation failed.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_expr_list_create(int argc);

/*!
 * \brief Resizes the number of children of the list.
 * \param list
 * \param argc The new number of children. If new children are added, they are
 * initialized to NULL. Existing children are unchanged. If the new size is
 * smaller than the old size, then the excessive children are recursively
 * destroyed.
 * \return Returns the new list.
 */
SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_expr_list_realloc(struct sfgsym_expr* list, int argc);

SFGSYM_PUBLIC_API struct sfgsym_expr*
sfgsym_expr_list_realloc_append(struct sfgsym_expr* list, struct sfgsym_expr* expr);

/*!
 * \brief Sets a child in the list.
 * \param list The list.
 * \param idx The index of the child to set. The list takes ownership of the
 * child. If there is an existing child, it will be recursively destroyed.
 * \param child The child to take ownership of.
 */
SFGSYM_PUBLIC_API void
sfgsym_expr_list_set(struct sfgsym_expr* list, int idx, struct sfgsym_expr* child);

SFGSYM_PUBLIC_API void
sfgsym_expr_destroy_recurse(struct sfgsym_expr* expr);

SFGSYM_PUBLIC_API int
sfgsym_expr_child_count(const struct sfgsym_expr* expr);

SFGSYM_PUBLIC_API void
sfgsym_expr_unlink_from_parent(struct sfgsym_expr* expr);

C_END
