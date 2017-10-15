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
    graph.setForwardPath(n1, n2);

    tfp::TransferFunction<double> tf = graph.calculateTransferFunction();
    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
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
