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

    Expression* mason() const;
    tfp::TransferFunction<double> calculateTransferFunction() const;

    bool evaluatePhysicalUnitConsistencies() const;

    void findForwardPathsAndLoops(PathList* paths, PathList* loops) const;
    void findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                           Node* current, NodeList list) const;
    void nodeListToPath(Path* path, const NodeList& nodes) const;
    Expression* calculatePathExpression(const Path& path) const;
    Expression* calculateGraphDeterminant(const PathList& loops) const;
    bool pathsAreTouching(const Path& a, const Path& b) const;

private:
    tfp::Reference<Node> input_;
    tfp::Reference<Node> output_;
    tfp::Reference<VariableTable> variables_;
};

} // namespace tfp
