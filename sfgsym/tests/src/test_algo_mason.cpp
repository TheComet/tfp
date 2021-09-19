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

    static sfgsym_expr* determinant(sfgsym_graph* g)
    {
        sfgsym_path_list loops;
        sfgsym_path_list_init(&loops);

        sfgsym_graph_find_loops(&loops, g);
        sfgsym_expr* tf = sfgsym_path_determinant_expr(&loops);

        sfgsym_path_list_deinit(&loops);

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

TEST_F(NAME, determinant_with_no_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));

    tf = determinant(&g);

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq(1));

    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, determinant_with_one_loop)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n1, parse_string("5"));

    tf = determinant(&g);

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq(1 - 3*5));

    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, determinant_with_two_touching_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));
    sfgsym_graph_connect_a_to_b(&g, n3, n2, parse_string("7"));
    sfgsym_graph_connect_a_to_b(&g, n2, n1, parse_string("11"));

    // L1: 5*7
    // L2: 3*11

    tf = determinant(&g);

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq(1 - 5*7 - 3*11));

    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, determinant_with_two_non_touching_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));
    sfgsym_graph_connect_a_to_b(&g, n3, n2, parse_string("7"));
    sfgsym_graph_connect_a_to_b(&g, n1, n4, parse_string("11"));
    sfgsym_graph_connect_a_to_b(&g, n4, n1, parse_string("13"));

    /*
     *                   P1
     * ---------------------------------------
     * 1 - L11*L12 - L21*L22 + L11*L12*L21*L22
     */

    tf = determinant(&g);

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq(1 - 5*7 - 11*13 + 5*7*11*13));

    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, determinant_with_three_non_touching_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&g, n2, n3, parse_string("5"));
    sfgsym_graph_connect_a_to_b(&g, n1, n1, parse_string("7"));
    sfgsym_graph_connect_a_to_b(&g, n2, n2, parse_string("11"));
    sfgsym_graph_connect_a_to_b(&g, n3, n3, parse_string("13"));

    /*
     * 1 - L1 - L2 - L3 + L1*L2 + L1*L3 + L2*L3 - L1*L2*L3
     */

    tf = determinant(&g);

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq(1 - 7 - 11 - 13 + 7*11 + 11*13 + 7*13 - 7*11*13));

    sfgsym_graph_deinit(&g);
}

TEST_F(NAME, mason_complicated_test)
{
    /*
     *  f       g               i
     *  <       <               <
     * / \     / \             / \
     * \ /  a  \ /  b       c  \ /  d
     *  o--->---o--->---o--->---o--->---o
     *   \      |   h   |              /
     *    \      \__<__/              /
     *     \            e            /
     *      \___________>___________/
     *
     *
     *          abcd + e(1 - g - hb - i + gi + bhi)
     * ----------------------------------------------------------
     * 1 - f - g - bh - i + fg + fhb + fi + gi + bhi - fgi - fhbi
     */
    sfgsym_graph graph;
    sfgsym_graph_init(&graph);

    sfgsym_node* n1 = sfgsym_graph_create_node(&graph, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&graph, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&graph, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&graph, "n4");
    sfgsym_node* n5 = sfgsym_graph_create_node(&graph, "n5");

    // loops
    sfgsym_graph_connect_a_to_b(&graph, n1, n1, parse_string("3"));
    sfgsym_graph_connect_a_to_b(&graph, n2, n2, parse_string("5"));
    sfgsym_graph_connect_a_to_b(&graph, n4, n4, parse_string("7"));
    sfgsym_graph_connect_a_to_b(&graph, n2, n3, parse_string("11"));
    sfgsym_graph_connect_a_to_b(&graph, n3, n2, parse_string("13"));

    // paths
    sfgsym_graph_connect_a_to_b(&graph, n1, n2, parse_string("17"));
    sfgsym_graph_connect_a_to_b(&graph, n3, n4, parse_string("19"));
    sfgsym_graph_connect_a_to_b(&graph, n4, n5, parse_string("23"));
    sfgsym_graph_connect_a_to_b(&graph, n1, n5, parse_string("29"));

    tf = mason(&graph, n1, n5);

    sfgsym_real a=17, b=11, c=19, d=23, e=29, f=3, g=5, h=13, i=7;
    sfgsym_real P1 = a*b*c*d;
    sfgsym_real P2 = e;
    sfgsym_real P2_det = 1 - g - h*b - i + g*i + b*h*i;
    sfgsym_real det = 1 - f - g - b*h - i + f*g + f*h*b + f*i + g*i + b*h*i - f*g*i - f*h*b*i;

    sfgsym_real result = sfgsym_expr_eval(tf, nullptr);
    EXPECT_THAT(result, DoubleEq((P1 + P2*P2_det) / det));

    sfgsym_graph_deinit(&graph);
}
