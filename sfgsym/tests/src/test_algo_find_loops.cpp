#include "sfgsym/algo/find_paths.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/branch.h"
#include "sfgsym/sfg/node.h"
#include <gmock/gmock.h>

#define NAME algo_find_loops

using namespace testing;

TEST(NAME, no_loops_no_islands)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(0));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, no_loops_islands)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
                      sfgsym_graph_create_node(&g, "n3");

    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(0));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, one_loop_three_nodes_no_islands)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n1, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(1));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(3));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l1, 2)->source->name, StrEq("n3"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, one_loop_two_nodes_islands)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
                      sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n1, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(1));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(2));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l1, 1)->source->name, StrEq("n2"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, one_loop_one_node_islands)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(1));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(1));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, two_self_loops_different_nodes)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n3, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(2));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(1));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));

    sfgsym_path* l2 = sfgsym_path_list_at(&l, 1);
    ASSERT_THAT(sfgsym_path_count(l2), Eq(1));
    ASSERT_THAT(sfgsym_path_at(l2, 0)->source->name, StrEq("n3"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, two_self_loops_same_node)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n1, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(2));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(1));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));

    sfgsym_path* l2 = sfgsym_path_list_at(&l, 1);
    ASSERT_THAT(sfgsym_path_count(l2), Eq(1));
    ASSERT_THAT(sfgsym_path_at(l2, 0)->source->name, StrEq("n1"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, two_connected_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n1, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(2));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(2));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l1, 1)->source->name, StrEq("n2"));

    sfgsym_path* l2 = sfgsym_path_list_at(&l, 1);
    ASSERT_THAT(sfgsym_path_count(l2), Eq(3));
    ASSERT_THAT(sfgsym_path_at(l2, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l2, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l2, 2)->source->name, StrEq("n3"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, sample_graph_1)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //
    // Cycles:
    // 1, 2, 3
    // 1, 5, 2, 3
    // 2, 3
    // 2, 3, 4, 5
    // 2, 3, 6, 4, 5
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");
    sfgsym_node* n5 = sfgsym_graph_create_node(&g, "n5");
    sfgsym_node* n6 = sfgsym_graph_create_node(&g, "n6");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n4, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n6, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n4, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n5, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n6, n4, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(5));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(3));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l1, 2)->source->name, StrEq("n3"));

    sfgsym_path* l2 = sfgsym_path_list_at(&l, 1);
    ASSERT_THAT(sfgsym_path_count(l2), Eq(4));
    ASSERT_THAT(sfgsym_path_at(l2, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l2, 1)->source->name, StrEq("n5"));
    ASSERT_THAT(sfgsym_path_at(l2, 2)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l2, 3)->source->name, StrEq("n3"));

    sfgsym_path* l3 = sfgsym_path_list_at(&l, 2);
    ASSERT_THAT(sfgsym_path_count(l3), Eq(2));
    ASSERT_THAT(sfgsym_path_at(l3, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l3, 1)->source->name, StrEq("n3"));

    sfgsym_path* l4 = sfgsym_path_list_at(&l, 3);
    ASSERT_THAT(sfgsym_path_count(l4), Eq(4));
    ASSERT_THAT(sfgsym_path_at(l4, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l4, 1)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(l4, 2)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(l4, 3)->source->name, StrEq("n5"));

    sfgsym_path* l5 = sfgsym_path_list_at(&l, 4);
    ASSERT_THAT(sfgsym_path_count(l5), Eq(5));
    ASSERT_THAT(sfgsym_path_at(l5, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l5, 1)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(l5, 2)->source->name, StrEq("n6"));
    ASSERT_THAT(sfgsym_path_at(l5, 3)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(l5, 4)->source->name, StrEq("n5"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, sample_graph_2)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //
    // Cycles:
    // 1, 2, 3
    // 1, 5, 2, 3
    // 2, 3
    // 2, 3, 4, 5
    // 2, 3, 6, 4, 5
    // 8, 9
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");
    sfgsym_node* n5 = sfgsym_graph_create_node(&g, "n5");
    sfgsym_node* n6 = sfgsym_graph_create_node(&g, "n6");
    sfgsym_node* n7 = sfgsym_graph_create_node(&g, "n7");
    sfgsym_node* n8 = sfgsym_graph_create_node(&g, "n8");
    sfgsym_node* n9 = sfgsym_graph_create_node(&g, "n9");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n8, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n7, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n9, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n4, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n6, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n4, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n5, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n6, n4, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n8, n9, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n9, n8, nullptr);

    sfgsym_path_list l;
    sfgsym_path_list_init(&l);
    ASSERT_THAT(sfgsym_algo_find_loops(&l, &g), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&l), Eq(6));

    sfgsym_path* l1 = sfgsym_path_list_at(&l, 0);
    ASSERT_THAT(sfgsym_path_count(l1), Eq(3));
    ASSERT_THAT(sfgsym_path_at(l1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l1, 2)->source->name, StrEq("n3"));

    sfgsym_path* l2 = sfgsym_path_list_at(&l, 1);
    ASSERT_THAT(sfgsym_path_count(l2), Eq(4));
    ASSERT_THAT(sfgsym_path_at(l2, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(l2, 1)->source->name, StrEq("n5"));
    ASSERT_THAT(sfgsym_path_at(l2, 2)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l2, 3)->source->name, StrEq("n3"));

    sfgsym_path* l3 = sfgsym_path_list_at(&l, 2);
    ASSERT_THAT(sfgsym_path_count(l3), Eq(2));
    ASSERT_THAT(sfgsym_path_at(l3, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l3, 1)->source->name, StrEq("n3"));

    sfgsym_path* l4 = sfgsym_path_list_at(&l, 3);
    ASSERT_THAT(sfgsym_path_count(l4), Eq(4));
    ASSERT_THAT(sfgsym_path_at(l4, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l4, 1)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(l4, 2)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(l4, 3)->source->name, StrEq("n5"));

    sfgsym_path* l5 = sfgsym_path_list_at(&l, 4);
    ASSERT_THAT(sfgsym_path_count(l5), Eq(5));
    ASSERT_THAT(sfgsym_path_at(l5, 0)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(l5, 1)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(l5, 2)->source->name, StrEq("n6"));
    ASSERT_THAT(sfgsym_path_at(l5, 3)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(l5, 4)->source->name, StrEq("n5"));

    sfgsym_path* l6 = sfgsym_path_list_at(&l, 5);
    ASSERT_THAT(sfgsym_path_count(l6), Eq(2));
    ASSERT_THAT(sfgsym_path_at(l6, 0)->source->name, StrEq("n8"));
    ASSERT_THAT(sfgsym_path_at(l6, 1)->source->name, StrEq("n9"));

    sfgsym_path_list_deinit(&l);
    sfgsym_graph_deinit(&g);
}
