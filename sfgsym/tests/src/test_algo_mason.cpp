#include <gmock/gmock.h>
#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/graph.h"
#include "sfgsym/parsers/expression_parser.h"
#include "sfgsym/symbolic/expression.h"
#include "sfgsym/exporters/expr2dot.h"
#include "sfgsym/symbolic/subs_table.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/algo/find_paths.h"
#include "sfgsym/algo/mason.h"

#define NAME algo_mason

using namespace testing;

class NAME : public Test
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        if (tf)
            sfgsym_expr_destroy_recurse(tf);
    }

    sfgsym_expr* parse_string(const char* str)
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

        if (tf)
        {
            const testing::TestInfo* info = UnitTest::GetInstance()->current_test_info();
            std::string filename = std::string(info->test_suite_name()) + "__" + info->name() + ".dot";
            FILE* fp = fopen(filename.c_str(), "w");
            sfgsym_export_expr_dot(tf, fp);
            fclose(fp);
        }

        return tf;
    }

    sfgsym_expr* tf = nullptr;
};

TEST_F(NAME, single_forward_path)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));

    tf = mason(&g, n1, n3);

    sfgsym_subs_table st;
    sfgsym_subs_table_init(&st);
    sfgsym_real result = sfgsym_expr_eval(tf, &st);
    EXPECT_THAT(result, DoubleEq(15.0));

    sfgsym_subs_table_deinit(&st);
    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, multiple_forward_paths)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));
    sfgsym_graph_connect_a_to_b(&g, n3, n4, parse_string("7"));
    sfgsym_graph_connect_a_to_b(&g, n2, n4, parse_string("11"));
    sfgsym_graph_connect_a_to_b(&g, n3, n4, parse_string("13"));

    tf = mason(&g, n1, n4);

    sfgsym_subs_table st;
    sfgsym_subs_table_init(&st);
    sfgsym_real result = sfgsym_expr_eval(tf, &st);
    EXPECT_THAT(result, DoubleEq(3*5*7 + 3*11 + 3*5*13));

    sfgsym_subs_table_deinit(&st);
    sfgsym_graph_deinit(&g);
}
