#pragma once

#include "tfp/util/Reference.hpp"
#include <vector>
#include <set>

namespace tfp {

class Node;
class Connection;

/*!
 * @brief Manages the creation and manipulation of graph nodes and provides
 * methods to calculate the graph's transfer function.
 */
class Graph : public RefCounted
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

    Graph();

    /*!
     * @brief Creates a new node with the given name.
     * @note Even though the returned Node object is refcounted, the graph
     * will always hold a reference to every created node, so if you can
     * guarantee that the graph object will outlive whatever is calling this
     * method, it won't be necessary for you to hold a reference to the
     * returned node.
     * @return Returns the new node object.
     */
    Node* createNode(const char* name);

    /*!
     * @brief Destroys the specified node
     */
    void destroyNode(Node* node);
    Node* findNode(const char* name);
    void renameNode(Node* node, const char* name);

    void setForwardPath(Node* in, Node* out);

    const PathList& paths() const;
    const PathList& loops() const;

    void markDirty();
    bool dirty() const;
    void update();
    bool evaluatePhysicalUnitConsistencies() const;
    Expression* expression() const;

private:
    Expression* doMason();
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
    Node* input_;
    Node* output_;
    Reference<Expression> graphExpression_;
    std::set< Reference<Node> > nodes_;
    bool dirty_;
};

} // namespace tfp
