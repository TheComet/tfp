#include <gmock/gmock.h>
#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/graph.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/symbolic/subs_table.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/algo/find_paths.h"
#include "sfgsym/algo/mason.h"

#define NAME mason_gain_rule

using namespace testing;

static sfgsym_expr* parse_string(const char* str)
{
    sfgsym_expr_parser parser;
    sfgsym_expr_parser_init(&parser);
    sfgsym_expr* expr = sfgsym_expr_from_string(&parser, str);
    sfgsym_expr_parser_deinit(&parser);
    return expr;
}

static sfgsym_expr* mason(sfgsym_graph* g, sfgsym_node* in, sfgsym_node* out)
{
    sfgsym_path_list paths, loops;
    sfgsym_path_list_init(&paths);
    sfgsym_path_list_init(&loops);

    sfgsym_graph_find_forward_paths(&paths, in, out);
    sfgsym_graph_find_loops(&loops, g);
    sfgsym_expr* tf = sfgsym_path_mason_expr(&paths, &loops);

    sfgsym_path_list_deinit(&loops);
    sfgsym_path_list_deinit(&paths);

    return tf;
}

TEST(NAME, single_forward_path)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("4"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));

    sfgsym_expr* tf = mason(&g, n1, n3);

    tf = sfgsym_expr_op_create(2, (sfgsym_real (*)())sfgsym_op_mul,
            sfgsym_expr_literal_create(4.0),
            sfgsym_expr_literal_create(5.0));

    sfgsym_subs_table st;
    sfgsym_subs_table_init(&st);
    sfgsym_real result = sfgsym_expr_eval(tf, &st);
    EXPECT_THAT(result, DoubleEq(20.0));

    sfgsym_expr_destroy_recurse(tf);
    sfgsym_subs_table_deinit(&st);
    sfgsym_graph_deinit(&g);
}
