#pragma once

#include "tfp/math/Expression.hpp"
#include "tfp/math/Node.hpp"
#include "tfp/util/Reference.hpp"
#include <vector>

namespace tfp {

class Connection;

class Dirtyable
{
public:
    Dirtyable() : dirty_(true) {}
    void markDirty() { dirty_ = true; }
    bool dirty() const { return dirty_; }

private:
    bool dirty_;
};

/*!
 * @brief Manages the creation and manipulation of graph nodes and provides
 * methods to calculate the graph's transfer function.
 */
class Graph : public RefCounted, public Dirtyable
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

    void setForwardPath(Node* in, Node* out);

    const PathList& paths() const;
    const PathList& loops() const;

    void markDirty();
    bool dirty() const;
    void update();

    /*!
     * @brief Computes the graph's transfer function expression and stores it
     * in graphExpression_. The graph is no longer dirty if this operation
     * succeeds.
     */
    //void update();
    bool evaluatePhysicalUnitConsistencies() const;
    Expression* expression();

private:
    void updatePathsAndLoops();
    void updateGraphExpression();

    void findForwardPathsAndLoopsRecursive(Node* current, NodeList list);
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
    std::vector< Reference<Node> > nodes_;
    PathList paths_;
    PathList loops_;
    bool dirty_;
};

} // namespace tfp
