#pragma once

namespace tfp {

class Node;

class Graph
{
public:
	void setForwardPath(Node* in, Node* out);
	void updateTransferFunction();
	bool evaluatePhysicalUnitConsistencies() const;

private:
	void findForwardPathsAndLoops();
	void findForwardPathsAndLoopsRecursive(Node* current, QSet<Node*>& visitedNodes);

private:
	Node* input_;
	Node* output_;

	QVector< QVector<Node*> > loops_;
	QVector< QVector<Node*> > paths_;
};

} // namespace tfp
