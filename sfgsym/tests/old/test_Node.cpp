#include "gmock/gmock.h"
#include "tfp/math/Connection.hpp"
#include "tfp/math/Graph.hpp"
#include "tfp/math/Node.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Node

using namespace testing;
using namespace tfp;

TEST(NAME, node_connect_to_another_node)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Connection* c = n1->connectTo(n2);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->targetNode(), Eq(n2));
    ASSERT_THAT(n1->outgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n1->outgoingConnections()[0].get(), Eq(c));
    EXPECT_THAT(n2->outgoingConnections().size(), Eq(0u));
}

TEST(NAME, node_connect_from_another_node)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Connection* c = n2->connectFrom(n1);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->targetNode(), Eq(n2));
    ASSERT_THAT(n1->outgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n1->outgoingConnections()[0].get(), Eq(c));
    EXPECT_THAT(n2->outgoingConnections().size(), Eq(0u));
}

TEST(NAME, node_connect_to_self)
{
    Graph graph;
    Node* n = graph.createNode("n");
    Connection* c = n->connectTo(n);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->targetNode(), Eq(n));
    ASSERT_THAT(n->outgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n->outgoingConnections()[0].get(), Eq(c));

    n->disconnectFrom(n);  // avoid memory leaks
}

TEST(NAME, two_node_loop)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    n1->connectTo(n2);
    n1->connectFrom(n2);

    EXPECT_THAT(n1->outgoingConnections()[0]->targetNode()
                  ->outgoingConnections()[0]->targetNode(), Eq(n1));

    n1->disconnectFrom(n2); // avoid memory leaks
}
