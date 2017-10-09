#include "gmock/gmock.h"
#include "../model/Graph.hpp"
#include "../model/Node.hpp"
#include "../model/Connection.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Graph

using namespace testing;
using namespace dpsfg;

TEST(NAME, one_node)
{
    tfp::Reference<Node> n = new Node;
    Graph graph;
    graph.setForwardPath(n, n);
    tfp::TransferFunction<double> tf = graph.calculateTransferFunction();

    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(0));
}

TEST(NAME, default_connections_are_factor_1)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    n1->connectTo(n2);
    n2->connectTo(n3);

    Graph graph;
    graph.setForwardPath(n1, n2);

    tfp::TransferFunction<double> tf = graph.calculateTransferFunction();
    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
}

TEST(NAME, three_node_forward_path)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    tfp::Reference<Connection> c1 = n1->connectTo(n2);
    tfp::Reference<Connection> c2 = n2->connectTo(n3);

    Graph graph;
    graph.setForwardPath(n1, n3);
    c1->setExpression("A+B");
    c2->setExpression("C+D");
}
