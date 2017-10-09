#include "Connection.hpp"
#include "Graph.hpp"
#include "Node.hpp"

using namespace dpsfg;

// ----------------------------------------------------------------------------
void Graph::setForwardPath(Node* in, Node* out)
{
    input_ = in;
    output_ = out;
}

// ----------------------------------------------------------------------------
tfp::TransferFunction<double> Graph::calculateTransferFunction()
{
    return tfp::TransferFunction<double>();
}

// ----------------------------------------------------------------------------
void Graph::updateTransferFunction()
{
    findForwardPathsAndLoops();
}

// ----------------------------------------------------------------------------
bool Graph::evaluatePhysicalUnitConsistencies() const
{
    return false;
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoops()
{
    paths_.clear();
    loops_.clear();

    findForwardPathsAndLoopsRecursive(input_, QVector<Node*>());
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoopsRecursive(Node* current, QVector<Node*> list)
{
    // Reaching the end means we've found a forward path
    if (current == output_)
    {
        list.push_back(current); // need last node
        paths_.push_back(getNodeConnections(list));
        return;
    }

    // Reaching a node we've already passed means we've found a loop
    for (QVector<Node*>::iterator it = list.begin(); it != list.end(); ++it)
        if (current == *it)
        {
            list.erase(list.begin(), it); // all nodes preceeding the one we hit aren't part of the loop
            loops_.push_back(getNodeConnections(list));
            return;
        }

    list.push_back(current);

    const QVector<Connection*>& connections = current->getOutgoingConnections();
    for (QVector<Connection*>::const_iterator it = connections.begin(); it != connections.end(); ++it)
    {
        Node* child = (*it)->getTargetNode();
        findForwardPathsAndLoopsRecursive(child, list);
    }
}

// ----------------------------------------------------------------------------
QVector<Connection*> Graph::getNodeConnections(const QVector<Node*>& nodes)
{
    QVector<Connection*> result;

    int i = 0;
    while (i < nodes.size())
    {
        // Accounts for when the list of connections forms a loop (first connected with last)
        Node* prev = i == 0 ? nodes[nodes.size() - 1] : nodes[i-1];
        Node* next = nodes[i];

        const QVector<Connection*>& connections = prev->getOutgoingConnections();
        for (QVector<Connection*>::const_iterator it = connections.begin(); it != connections.end(); ++it)
            if ((*it)->getTargetNode() == next)
            {
                result.push_back(*it);
                break;
            }
    }

    return result;
}

// ----------------------------------------------------------------------------
void Graph::doMasonsGainFormula()
{

}

// ----------------------------------------------------------------------------
QString Graph::multiplyPathExpressions(const QVector<Connection*>& connections)
{
    QString result;
    for (QVector<Connection*>::const_iterator it = connections.begin(); it != connections.end(); ++it)
    {
        result += (*it)->getExpression();
        result += "*"; // we're multiplying
    }
    return result;
}
