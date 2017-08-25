#include "Graph.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
void Graph::setForwardPath(Node* in, Node* out)
{
	input_ = in;
	output_ = out;
}

// ----------------------------------------------------------------------------
void Graph::updateTransferFunction()
{
	findPathsAndLoops();
}

// ----------------------------------------------------------------------------
bool Graph::evaluatePhysicalUnitConsistencies() const
{

}

// ----------------------------------------------------------------------------
void Graph::findPathsAndLoops()
{
	paths_.clear();
	loops_.clear();
	QSet<Node*> visitedNodes;
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoopsRecursive(Node* current, QSet<Node*>& visitedNodes)
{
	const QVector<Node*>& connections = current->getOutgoingConnections();
	for (QVector<Node*>::iterator it = connections.begin(); )
}
