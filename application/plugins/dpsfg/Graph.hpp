#pragma once

#include <QVector>

namespace tfp {

class Node;
class Connection;

class Graph
{
public:
    void setForwardPath(Node* in, Node* out);
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
};

} // namespace tfp
