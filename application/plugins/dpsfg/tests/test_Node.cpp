#include "gmock/gmock.h"
#include "../model/Node.hpp"
#include "../model/Connection.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Node

using namespace testing;
using namespace dpsfg;

TEST(NAME, node_connect_to_another_node)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Connection> c = n1->connectTo(n2);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n2));
    ASSERT_THAT(n1->getOutgoingConnections().size(), Eq(1));
    EXPECT_THAT(n1->getOutgoingConnections()[0], Eq(c));
    EXPECT_THAT(n2->getOutgoingConnections().size(), Eq(0));
}

TEST(NAME, node_connect_from_another_node)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Connection> c = n2->connectFrom(n1);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n2));
    ASSERT_THAT(n1->getOutgoingConnections().size(), Eq(1));
    EXPECT_THAT(n1->getOutgoingConnections()[0], Eq(c));
    EXPECT_THAT(n2->getOutgoingConnections().size(), Eq(0));
}

TEST(NAME, node_connect_to_self)
{
    tfp::Reference<Node> n = new Node;
    tfp::Reference<Connection> c = n->connectTo(n);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n));
    ASSERT_THAT(n->getOutgoingConnections().size(), Eq(1));
    EXPECT_THAT(n->getOutgoingConnections()[0], Eq(c));

    n->disconnectFrom(n);  // avoid memory leaks
}

TEST(NAME, two_node_loop)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    n1->connectTo(n2);
    n1->connectFrom(n2);

    EXPECT_THAT(n1->getOutgoingConnections()[0]->getTargetNode()
                  ->getOutgoingConnections()[0]->getTargetNode(), Eq(n1));

    n1->disconnectFrom(n2); // avoid memory leaks
}
