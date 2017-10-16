#include "gmock/gmock.h"
#include "model/Connection.hpp"
#include "model/Expression.hpp"
#include "model/Graph.hpp"
#include "model/Node.hpp"
#include "model/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME dpsfg_Graph

using namespace testing;
using namespace dpsfg;

TEST(NAME, single_node_forward_path_returns_identity_TF)
{
    tfp::Reference<Node> n = new Node;
    Graph graph;
    graph.setForwardPath(n, n);
    tfp::TransferFunction<double> tf = graph.calculateTransferFunction();

    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
}

TEST(NAME, default_connections_are_identity_TF)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    n1->connectTo(n2);
    n2->connectTo(n3);

    Graph graph;
    graph.setForwardPath(n1, n3);

    tfp::TransferFunction<double> tf = graph.calculateTransferFunction();
    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
}

TEST(NAME, find_single_forward_path)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    tfp::Reference<Connection> c1 = n1->connectTo(n2);
    tfp::Reference<Connection> c2 = n2->connectTo(n3);

    Graph graph;
    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n3);
    graph.findForwardPathsAndLoops(&paths, &loops);
    EXPECT_THAT(loops.size(), Eq(0u));
    ASSERT_THAT(paths.size(), Eq(1u));
    ASSERT_THAT(paths[0].size(), Eq(2u));
    EXPECT_THAT(paths[0][0], Eq(c1));
    EXPECT_THAT(paths[0][1], Eq(c2));
}

TEST(NAME, find_multiple_forward_paths)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    tfp::Reference<Node> n4 = new Node;
    tfp::Reference<Node> n5 = new Node;
    tfp::Reference<Node> n6 = new Node;
    tfp::Reference<Node> n7 = new Node;
    tfp::Reference<Connection> c1 = n1->connectTo(n2);
    tfp::Reference<Connection> c2 = n2->connectTo(n3);
    tfp::Reference<Connection> c3 = n3->connectTo(n4);
    tfp::Reference<Connection> c4 = n4->connectTo(n5);
    tfp::Reference<Connection> c5 = n2->connectTo(n6);
    tfp::Reference<Connection> c6 = n6->connectTo(n7);
    tfp::Reference<Connection> c7 = n7->connectTo(n5);

    /*
     *            --> n6 --> n7 --
     *          /                  v
     * n1 --> n2 --> n3 --> n4 --> n5
     *
     */

    Graph graph;
    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n5);
    graph.findForwardPathsAndLoops(&paths, &loops);
    EXPECT_THAT(loops.size(), Eq(0u));
    ASSERT_THAT(paths.size(), Eq(2u));

    ASSERT_THAT(paths[0].size(), Eq(4u));
    EXPECT_THAT(paths[0][0], Eq(c1));
    EXPECT_THAT(paths[0][1], Eq(c2));
    EXPECT_THAT(paths[0][2], Eq(c3));
    EXPECT_THAT(paths[0][3], Eq(c4));

    ASSERT_THAT(paths[1].size(), Eq(4u));
    EXPECT_THAT(paths[1][0], Eq(c1));
    EXPECT_THAT(paths[1][1], Eq(c5));
    EXPECT_THAT(paths[1][2], Eq(c6));
    EXPECT_THAT(paths[1][3], Eq(c7));
}

TEST(NAME, find_single_loop)
{
    tfp::Reference<Node> n1 = new Node;
    tfp::Reference<Node> n2 = new Node;
    tfp::Reference<Node> n3 = new Node;
    tfp::Reference<Connection> c1 = n1->connectTo(n2);
    tfp::Reference<Connection> c2 = n2->connectTo(n3);
    tfp::Reference<Connection> c3 = n3->connectTo(n2);

    Graph graph;
    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n3);
    graph.findForwardPathsAndLoops(&paths, &loops);
    ASSERT_THAT(loops.size(), Eq(1u));
    ASSERT_THAT(paths.size(), Eq(1u));

    ASSERT_THAT(paths[0].size(), Eq(2u));
    EXPECT_THAT(paths[0][0], Eq(c1));
    EXPECT_THAT(paths[0][1], Eq(c2));

    ASSERT_THAT(loops[0].size(), Eq(2u));
    EXPECT_THAT(loops[0][0], Eq(c2));
    EXPECT_THAT(loops[0][1], Eq(c3));
}

TEST(NAME, not_really_a_test_yet)
{
    tfp::Reference<Node> V1 = new Node;
    tfp::Reference<Node> I2 = new Node;
    tfp::Reference<Node> V2 = new Node;
    tfp::Reference<Node> V3 = new Node;
    V1->connectTo(I2)->setExpression(Expression::make("G1"));
    I2->connectTo(V2)->setExpression(Expression::parse("G1+G2+s*C"));
    V2->connectTo(V3)->setExpression(Expression::parse("-A"));
    V3->connectTo(I2)->setExpression(Expression::parse("G2+s*C"));

    Graph graph;
    graph.setForwardPath(V1, V3);
    tfp::Reference<Expression> graphExpression = graph.mason();
    tfp::Reference<VariableTable> variables = graphExpression->generateVariableTable();
    variables->set("G1", "1/R1");
    variables->set("G2", "1/R2");
    variables->set("R1", 1e3);
    variables->set("R2", 1e3);
    variables->set("C", 1e-6);
    variables->set("A", "inf");
    Expression::TransferFunctionCoefficients tfc = graphExpression->calculateTransferFunctionCoefficients("s");

    tfp::TransferFunction<double> tf = graphExpression->calculateTransferFunction(tfc, variables);
    double freq[100], amp[100], phase[100];
    tf.frequencyResponse(freq, amp, phase, 1e2, 1e4, 100);
}
