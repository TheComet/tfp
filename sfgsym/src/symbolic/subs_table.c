#include "sfgsym/symbolic/subs_table.h"
#include <string.h>

/* ------------------------------------------------------------------------- */
static cs_hash32
hash32_string_jenkins_oaat(const void* key)
{
    cs_hash32 hash;
    const char* c = *(const char**)key;

    for(hash = 0; *c; ++c)
    {
        hash += (uint8_t)*c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 1);
    hash += (hash << 15);

    return hash;
}

/* ------------------------------------------------------------------------- */
static int
compare_keys(const void* a, const void* b)
{
    return strcmp((const char*)a, (const char*)b);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_subs_table_init(struct sfgsym_subs_table* st)
{
    if (varhashmap_init_with_options(
                &st->table,
                sizeof(char*),
                hash32_string_jenkins_oaat,
                compare_keys,
                sizeof(struct sfgsym_expr*)) != HM_OK)
    {
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
SFGSYM_PUBLIC_API void
sfgsym_subs_table_deinit(struct sfgsym_subs_table* st)
{
    varhashmap_deinit(&st->table);
}

/* ------------------------------------------------------------------------- */
sfgsym_real
sfgsym_subs_table_eval_variable(
        const struct sfgsym_subs_table* st,
        const char* varname)
{
    varhashmap_find(&st->table, varname);
}
