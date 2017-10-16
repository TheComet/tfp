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
    struct PathSegment
    {
        Node* in_;
        Node* out_;
        Connection* connection_;
    };

    typedef std::vector<PathSegment> Path;
    typedef std::vector<Path> PathList;
    typedef std::vector<Node*> NodeList;

    void setForwardPath(Node* in, Node* out);
    VariableTable* variables();

    Expression* mason() const;
    tfp::TransferFunction<double> calculateTransferFunction() const;

    bool evaluatePhysicalUnitConsistencies() const;

    void findForwardPathsAndLoops(PathList* paths, PathList* loops) const;
    void findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                           Node* current, NodeList list) const;
    void nodeListToPath(Path* path, const NodeList& nodes) const;
    Expression* calculateConnectionGain(const Path& path) const;
    Expression* calculateDeterminant(const PathList& loops) const;
    Expression* calculateCofactorsAndPathGains(const PathList& loops, const PathList& paths) const;
    bool pathsAreTouching(const Path& a, const Path& b) const;

    void dump(const char* fileName) const;
    void dump(FILE* fileName) const;

private:
    tfp::Reference<Node> input_;
    tfp::Reference<Node> output_;
    tfp::Reference<VariableTable> variables_;
};

} // namespace tfp
