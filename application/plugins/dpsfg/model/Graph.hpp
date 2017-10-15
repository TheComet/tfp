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
    typedef std::vector< tfp::Reference<Connection> > Path;
    typedef std::vector<Node*> NodeList;
    typedef std::vector<Path> PathList;

    void setForwardPath(Node* in, Node* out);
    VariableTable* variables();

    Expression* mason();
    tfp::TransferFunction<double> calculateTransferFunction();

    bool evaluatePhysicalUnitConsistencies() const;

private:
    void findForwardPathsAndLoops(PathList* paths, PathList* loops);
    void findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                           Node* current, NodeList list);
    void nodeListToPath(Path* path, const NodeList& nodes);
    Expression* calculatePathExpression(const Path& path);

private:
    tfp::Reference<Node> input_;
    tfp::Reference<Node> output_;
    tfp::Reference<VariableTable> variables_;
};

} // namespace tfp
