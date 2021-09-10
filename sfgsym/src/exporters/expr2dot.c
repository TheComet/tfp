#include "sfgsym/exporters/expr2dot.h"
#include "sfgsym/symbolic/expression.h"
#include "cstructures/btree.h"

/* ------------------------------------------------------------------------- */
static int
map_guids_recurse(struct cs_btree* guids, const struct sfgsym_expr* expr, int* counter)
{
    int i, child_count;

    if (expr == NULL)
        return 0;

    (*counter)++;
    if (btree_insert_new(
                guids,
                (cs_btree_key)(uintptr_t)expr,
                counter) != BTREE_OK)
    {
        return -1;
    }

    child_count = sfgsym_expr_child_count(expr);
    for (i = 0; i < child_count; ++i)
        if (map_guids_recurse(guids, expr->child[i], counter) != 0)
            return -1;

    return 0;
}
static int
map_guids(struct cs_btree* guids, const struct sfgsym_expr* expr)
{
    int counter = 0;
    return map_guids_recurse(guids, expr, &counter);
}

/* ------------------------------------------------------------------------- */
static void
write_edge(FILE* fp,
           const struct sfgsym_expr* from,
           const struct sfgsym_expr* to,
           int edge_idx,
           const struct cs_btree* guids)
{
    int from_id = *(int*)btree_find(guids, (cs_btree_key)(uintptr_t)from);
    int to_id = *(int*)btree_find(guids, (cs_btree_key)(uintptr_t)to);
    fprintf(fp, "    N%d -> N%d [label=\"%d\"];\n", from_id, to_id, edge_idx);
    fprintf(fp, "    N%d -> N%d [color=\"blue\"];\n", to_id, from_id);
}

/* ------------------------------------------------------------------------- */
static void
write_edges(FILE* fp,
            const struct sfgsym_expr* expr,
            const struct cs_btree* guids)
{
    int i, child_count;
    child_count = sfgsym_expr_child_count(expr);
    for (i = 0; i < child_count; ++i)
    {
        if (expr->child[i] == NULL)
            continue;

        write_edge(fp, expr, expr->child[i], i, guids);
        write_edges(fp, expr->child[i], guids);
    }
}

/* ------------------------------------------------------------------------- */
static void
write_nodes(FILE* fp, const struct sfgsym_expr* expr, const struct cs_btree* guids)
{
    int i, child_count, node_id;

    if (expr == NULL)
        return;

    child_count = sfgsym_expr_child_count(expr);
    node_id = *(int*)btree_find(guids, (cs_btree_key)(uintptr_t)expr);
    fprintf(fp, "    N%d [label=\"", node_id);
    switch (expr->type)
    {
        case SFGSYM_LITERAL  : fprintf(fp, "%f", expr->data.literal); break;
        case SFGSYM_VARIABLE : fprintf(fp, "%s", expr->data.varname); break;
        case SFGSYM_INFINITY : fprintf(fp, "oo"); break;
        default : {
            if (expr->data.op == NULL)
                fprintf(fp, "list[%d]", child_count);
            else
            {
                if      (expr->data.op == sfgsym_op_add) fprintf(fp, "+");
                else if (expr->data.op == sfgsym_op_sub) fprintf(fp, "-");
                else if (expr->data.op == sfgsym_op_mul) fprintf(fp, "*");
                else if (expr->data.op == sfgsym_op_div) fprintf(fp, "/");
                else if (expr->data.op == sfgsym_op_pow) fprintf(fp, "^");
                else
                {
                    fprintf(fp, "op%d", child_count);
                }
            }
        } break;
    }
    fprintf(fp, "\"];\n");

    for (i = 0; i < child_count; ++i)
        write_nodes(fp, expr->child[i], guids);
}

/* ------------------------------------------------------------------------- */
int
sfgsym_export_expr_dot_file(const struct sfgsym_expr* expr, const char* file_name)
{
    FILE* fp = fopen(file_name, "w");
    if (fp == NULL)
        goto open_file_failed;

    if (sfgsym_export_expr_dot(expr, fp) != 0)
        goto export_failed;

    fclose(fp);
    return 0;

    export_failed    : fclose(fp);
    open_file_failed : return -1;
}

/* ------------------------------------------------------------------------- */
int
sfgsym_export_expr_dot(const struct sfgsym_expr* expr, FILE* fp)
{
    struct cs_btree guids;
    btree_init(&guids, sizeof(int));
    if (map_guids(&guids, expr) != 0)
        goto map_guids_failed;

    fprintf(fp, "digraph name {\n");
    write_edges(fp, expr, &guids);
    write_nodes(fp, expr, &guids);
    fprintf(fp, "}\n");

    btree_deinit(&guids);

    return 0;

    map_guids_failed : return -1;
}
