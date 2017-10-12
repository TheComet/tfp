#pragma once

#include "tfp/util/Reference.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVector>

namespace dpsfg {

class Node;
class Connection;

class Graph : public tfp::RefCounted
{
public:
    void setForwardPath(Node* in, Node* out);
    tfp::TransferFunction<double> calculateTransferFunction();
    void setDefaultValue(const QString& varName, double value);

    void updateTransferFunction();
    bool evaluatePhysicalUnitConsistencies() const;

private:
    void findForwardPathsAndLoops();
    void findForwardPathsAndLoopsRecursive(Node* current, QVector<Node*> list);
    QVector<Connection*> getNodeConnections(const QVector<Node*>& nodes);
    void doMasonsGainFormula();
    QString multiplyPathExpressions(const QVector<Connection*>& connections);

private:
    tfp::Reference<Node> input_;
    tfp::Reference<Node> output_;

    QVector< QVector<Connection*> > loops_;
    QVector< QVector<Connection*> > paths_;
    tfp::TransferFunction<double> transferFunction_;
    QString expression_;
};

} // namespace tfp
