#pragma once

#include "sfgsym/config.h"
#include "cstructures/varhashmap.h"

C_BEGIN

struct sfgsym_expr;

struct sfgsym_subs_table
{
    struct cs_varhashmap table;
};

SFGSYM_PUBLIC_API int
sfgsym_subs_table_init(struct sfgsym_subs_table* st);

SFGSYM_PUBLIC_API void
sfgsym_subs_table_deinit(struct sfgsym_subs_table* st);

SFGSYM_PUBLIC_API sfgsym_real
sfgsym_subs_table_eval_variable(
        const struct sfgsym_subs_table* st,
        const char* varname);

C_END
