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
bool Graph::evaluatePhysicalUnitConsistencies() const
{
    return false;
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoops(PathList* paths, PathList* loops)
{
    findForwardPathsAndLoopsRecursive(paths, loops, input_, NodeList());
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                              Node* current, NodeList list)
{
    // Reaching a node we've already passed means we've found a loop
    for (NodeList::iterator it = list.begin(); it != list.end(); ++it)
        if (current == *it)
        {
            list.erase(list.begin(), it); // all nodes preceeding the one we hit aren't part of the loop
            Path path;
            nodeListToPath(&path, list);
            loops->push_back(path);
            return;
        }

    // Do this before checking for path goal, as it belongs to the path
    list.push_back(current);

    // Reaching the end means we've found a forward path
    if (current == output_)
    {
        Path path;
        nodeListToPath(&path, list);
        paths->push_back(path);
    }

    const Node::ConnectionList& connections = current->getOutgoingConnections();
    for (Node::ConnectionList::const_iterator it = connections.begin(); it != connections.end(); ++it)
    {
        Node* child = (*it)->getTargetNode();
        findForwardPathsAndLoopsRecursive(paths, loops, child, list);
    }
}

// ----------------------------------------------------------------------------
void Graph::nodeListToPath(Path* path, const NodeList& nodes)
{
    for (std::size_t i = 0; i < nodes.size(); ++i)
    {
        // Accounts for when the list of connections forms a loop (first connected with last)
        Node* curr = nodes[i];
        Node* next = i == nodes.size() - 1 ? nodes[0] : nodes[i+1];

        const Node::ConnectionList& connections = curr->getOutgoingConnections();
        for (Node::ConnectionList::const_iterator it = connections.begin(); it != connections.end(); ++it)
            if ((*it)->getTargetNode() == next)
            {
                path->push_back(*it);
                break;
            }
    }
}

// ----------------------------------------------------------------------------
Expression* Graph::mason()
{
    PathList paths;
    PathList loops;
    findForwardPathsAndLoops(&paths, &loops);
    return Expression::make(1.0);
}

// ----------------------------------------------------------------------------
Expression* Graph::calculatePathExpression(const Path& path)
{
    // Multiply all path expressions together
    Expression* e = NULL;
    for (Path::const_iterator connection = path.begin(); connection != path.end(); ++connection)
    {
        if (e == NULL)
            e = (*connection)->getExpression();
        else
            e = Expression::make(op::mul, (*connection)->getExpression(), e);
    }

    return e;
}
