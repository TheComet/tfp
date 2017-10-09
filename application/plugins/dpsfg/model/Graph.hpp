#pragma once

#include "tfp/util/RefCounted.hpp"
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

    void updateTransferFunction();
    bool evaluatePhysicalUnitConsistencies() const;

private:
    void findForwardPathsAndLoops();
    void findForwardPathsAndLoopsRecursive(Node* current, QVector<Node*> list);
    QVector<Connection*> getNodeConnections(const QVector<Node*>& nodes);
    void doMasonsGainFormula();
    QString multiplyPathExpressions(const QVector<Connection*>& connections);

private:
    Node* input_;
    Node* output_;

    QVector< QVector<Connection*> > loops_;
    QVector< QVector<Connection*> > paths_;
    QString expression_;
    tfp::TransferFunction<double> transferFunction_;
};

} // namespace tfp
