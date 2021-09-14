#include "sfgsym/algo/find_paths.h"
#include "sfgsym/sfg/path.h"
#include "sfgsym/sfg/graph.h"
#include "sfgsym/sfg/branch.h"
#include "sfgsym/sfg/node.h"
#include <gmock/gmock.h>

#define NAME algo_find_paths

using namespace testing;

TEST(NAME, find_single_forward_path)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //
    //    n1 --> n2 --> n3
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);

    sfgsym_path_list p;
    sfgsym_path_list_init(&p);
    ASSERT_THAT(sfgsym_algo_find_forward_paths(&p, n1, n3), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&p), Eq(1));

    sfgsym_path* p1 = sfgsym_path_list_at(&p, 0);
    ASSERT_THAT(sfgsym_path_count(p1), Eq(2));
    ASSERT_THAT(sfgsym_path_at(p1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->dest->name, StrEq("n3"));

    sfgsym_path_list_deinit(&p);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, find_multiple_forward_paths)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //
    //            --> n6 --> n7 --
    //          /                  v
    // n1 --> n2 --> n3 --> n4 --> n5
    //
    //
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");
    sfgsym_node* n5 = sfgsym_graph_create_node(&g, "n5");
    sfgsym_node* n6 = sfgsym_graph_create_node(&g, "n6");
    sfgsym_node* n7 = sfgsym_graph_create_node(&g, "n7");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n4, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n4, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n6, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n6, n7, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n7, n5, nullptr);


    sfgsym_path_list p;
    sfgsym_path_list_init(&p);
    ASSERT_THAT(sfgsym_algo_find_forward_paths(&p, n1, n5), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&p), Eq(2));

    sfgsym_path* p1 = sfgsym_path_list_at(&p, 0);
    ASSERT_THAT(sfgsym_path_count(p1), Eq(4));
    ASSERT_THAT(sfgsym_path_at(p1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p1, 2)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(p1, 3)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(p1, 3)->dest->name, StrEq("n5"));

    sfgsym_path* p2 = sfgsym_path_list_at(&p, 1);
    ASSERT_THAT(sfgsym_path_count(p2), Eq(4));
    ASSERT_THAT(sfgsym_path_at(p2, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p2, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p2, 2)->source->name, StrEq("n6"));
    ASSERT_THAT(sfgsym_path_at(p2, 3)->source->name, StrEq("n7"));
    ASSERT_THAT(sfgsym_path_at(p2, 3)->dest->name, StrEq("n5"));

    sfgsym_path_list_deinit(&p);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, find_single_forward_path_with_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //        ___<__
    //       /   /\ \     .
    //      /    \/ /
    //    n1 --> n2 --> n3
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n1, nullptr);

    sfgsym_path_list p;
    sfgsym_path_list_init(&p);
    ASSERT_THAT(sfgsym_algo_find_forward_paths(&p, n1, n3), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&p), Eq(1));

    sfgsym_path* p1 = sfgsym_path_list_at(&p, 0);
    ASSERT_THAT(sfgsym_path_count(p1), Eq(2));
    ASSERT_THAT(sfgsym_path_at(p1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->dest->name, StrEq("n3"));

    sfgsym_path_list_deinit(&p);
    sfgsym_graph_deinit(&g);
}

TEST(NAME, find_multiple_forward_paths_with_loops)
{
    sfgsym_graph g;
    sfgsym_graph_init(&g);

    //
    // /\         --> n6 --> n7 --\                 .
    // \/        /                 v
    // n1 --> n2 --> n3 --> n4 --> n5
    //         ^           /
    //          \--- n8 <--
    //
    sfgsym_node* n1 = sfgsym_graph_create_node(&g, "n1");
    sfgsym_node* n2 = sfgsym_graph_create_node(&g, "n2");
    sfgsym_node* n3 = sfgsym_graph_create_node(&g, "n3");
    sfgsym_node* n4 = sfgsym_graph_create_node(&g, "n4");
    sfgsym_node* n5 = sfgsym_graph_create_node(&g, "n5");
    sfgsym_node* n6 = sfgsym_graph_create_node(&g, "n6");
    sfgsym_node* n7 = sfgsym_graph_create_node(&g, "n7");
    sfgsym_node* n8 = sfgsym_graph_create_node(&g, "n8");
    sfgsym_graph_connect_a_to_b(&g, n1, n1, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n1, n2, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n3, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n3, n4, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n4, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n4, n8, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n2, n6, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n6, n7, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n7, n5, nullptr);
    sfgsym_graph_connect_a_to_b(&g, n8, n2, nullptr);

    sfgsym_path_list p;
    sfgsym_path_list_init(&p);
    ASSERT_THAT(sfgsym_algo_find_forward_paths(&p, n1, n5), Eq(0));
    ASSERT_THAT(sfgsym_path_list_count(&p), Eq(2));

    sfgsym_path* p1 = sfgsym_path_list_at(&p, 0);
    ASSERT_THAT(sfgsym_path_count(p1), Eq(4));
    ASSERT_THAT(sfgsym_path_at(p1, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p1, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p1, 2)->source->name, StrEq("n3"));
    ASSERT_THAT(sfgsym_path_at(p1, 3)->source->name, StrEq("n4"));
    ASSERT_THAT(sfgsym_path_at(p1, 3)->dest->name, StrEq("n5"));

    sfgsym_path* p2 = sfgsym_path_list_at(&p, 1);
    ASSERT_THAT(sfgsym_path_count(p2), Eq(4));
    ASSERT_THAT(sfgsym_path_at(p2, 0)->source->name, StrEq("n1"));
    ASSERT_THAT(sfgsym_path_at(p2, 1)->source->name, StrEq("n2"));
    ASSERT_THAT(sfgsym_path_at(p2, 2)->source->name, StrEq("n6"));
    ASSERT_THAT(sfgsym_path_at(p2, 3)->source->name, StrEq("n7"));
    ASSERT_THAT(sfgsym_path_at(p2, 3)->dest->name, StrEq("n5"));

    sfgsym_path_list_deinit(&p);
    sfgsym_graph_deinit(&g);
}
