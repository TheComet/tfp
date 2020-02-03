#include "sfgsym/sfg/Connection.hpp"
#include "sfgsym/sfg/Graph.hpp"
#include "sfgsym/util/NChooseK.hpp"

namespace sfgsym {

// ----------------------------------------------------------------------------
Graph::Graph() :
    input_(NULL),
    output_(NULL),
    dirty_(false)
{
}

// ----------------------------------------------------------------------------
Node* Graph::createNode(const char* name)
{
    nodes_.push_back(Reference<Node>(new Node(this, name)));
    return nodes_.back();
}

// ----------------------------------------------------------------------------
void Graph::destroyNode(Node* node)
{
    for (std::vector<Reference<Node>>::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
        if (it->get() == node)
        {
            nodes_.erase(it);
            return;
        }
}

// ----------------------------------------------------------------------------
Node* Graph::findNode(const char* name)
{
    for (std::vector<Reference<Node>>::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
        if (it->get()->name() == name)
            return it->get();
    return NULL;
}

// ----------------------------------------------------------------------------
void Graph::setForwardPath(Node* in, Node* out)
{
    input_ = in;
    output_ = out;
    markDirty();
}

// ----------------------------------------------------------------------------
const Graph::PathList& Graph::paths() const
{
    return paths_;
}

// ----------------------------------------------------------------------------
const Graph::PathList& Graph::loops() const
{
    return loops_;
}

// ----------------------------------------------------------------------------
void Graph::markDirty()
{
    dirty_ = true;
}

// ----------------------------------------------------------------------------
bool Graph::dirty() const
{
    return dirty_;
}

// ----------------------------------------------------------------------------
void Graph::update()
{
    updatePathsAndLoops();
    updateGraphExpression();
    dirty_ = false;
}

// ----------------------------------------------------------------------------
bool Graph::evaluatePhysicalUnitConsistencies() const
{
    return false;
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoopsRecursive(Node* current, NodeList list)
{
    // Reaching a node we've already passed means we've found a loop
    for (NodeList::iterator it = list.begin(); it != list.end(); ++it)
        if (current == *it)
        {
            list.erase(list.begin(), it); // all nodes preceeding the one we hit aren't part of the loop
            Path path;
            nodeListToPath(&path, list);
            loops_.push_back(path);
            return;
        }

    // Do this before checking for path goal, as it belongs to the path
    list.push_back(current);

    // Reaching the end means we've found a forward path
    if (current == output_)
    {
        Path path;
        nodeListToPath(&path, list);
        paths_.push_back(path);
    }

    const Node::ConnectionList& connections = current->outgoingConnections();
    for (Node::ConnectionList::const_iterator it = connections.begin(); it != connections.end(); ++it)
    {
        Node* child = (*it)->targetNode();
        findForwardPathsAndLoopsRecursive(child, list);
    }
}

// ----------------------------------------------------------------------------
void Graph::nodeListToPath(Path* path, const NodeList& nodes) const
{
    for (std::size_t i = 0; i < nodes.size(); ++i)
    {
        // Accounts for when the list of connections forms a loop (first connected with last)
        Node* curr = nodes[i];
        Node* next = i == nodes.size() - 1 ? nodes[0] : nodes[i+1];

        const Node::ConnectionList& connections = curr->outgoingConnections();
        for (Node::ConnectionList::const_iterator it = connections.begin(); it != connections.end(); ++it)
            if ((*it)->targetNode() == next)
            {
                PathSegment segment;
                segment.connection_ = it->get();
                segment.in_ = curr;
                segment.out_ = next;
                path->push_back(segment);
                break;
            }
    }
}

// ----------------------------------------------------------------------------
void Graph::updatePathsAndLoops()
{
    paths_.clear();
    loops_.clear();

    findForwardPathsAndLoopsRecursive(input_, NodeList());
}

// ----------------------------------------------------------------------------
void Graph::updateGraphExpression()
{
    Expression* graphDeterminant = calculateDeterminant(loops_);
    Expression* forwardGain = calculateCofactorsAndPathGains(paths_, loops_);

    graphExpression_ = Expression::make(op::div, forwardGain, graphDeterminant);
}

// ----------------------------------------------------------------------------
Expression* Graph::calculateConnectionGain(const Path& path) const
{
    // Multiply all path expressions together
    Expression* e = path[0].connection_->expression();
    for (std::size_t i = 1; i < path.size(); ++i)
    {
        e = Expression::make(op::mul, e, path[i].connection_->expression());
    }

    return e;
}

// ----------------------------------------------------------------------------
Expression* Graph::calculateDeterminant(const PathList& loops) const
{
    // Nothing to do if there are no loops
    if (loops.size() == 0)
        return Expression::make(1.0);

    /*
     * Multiply all gain factors in each loop now, because these expressions
     * could potentially be used hundreds of times.
     */
    std::vector< sfgsym::Reference<Expression> > loopExpressions(loops.size());
    for (std::size_t i = 0; i != loops.size(); ++i)
        loopExpressions[i] = calculateConnectionGain(loops[i]);

    /*
     * For k=0 and k=1 the expressions are trivial and can be computed manually
     * as follows: 1 - L1 - L2 - ... - Li  (where Li is the loop gain at index i)
     */
    Expression* e = Expression::make(1.0);
    for (std::size_t i = 0; i != loops.size(); ++i)
        e = Expression::make(op::sub, e, loopExpressions[i]);

    // If there are less than two loops, we're done
    if (loops.size() < 2)
        return e;

    /*
     * Create a table of all combinations of loops touching/not touching each
     * other, because pathsAreTouching() is fairly expensive compared to a
     * two lookups in two vectors.
     */
    std::vector< std::vector<bool> > touchingLoops(loops.size() - 1);
    for (std::size_t i = 0; i != loops.size(); ++i)
        for (std::size_t j = i+1; j < loops.size(); ++j)
            touchingLoops[i].push_back(pathsAreTouching(loops[i], loops[j]));

    std::vector<int> permutation(loops.size());
    std::vector<int> subPermutation(loops.size());
    for (std::size_t k = 2; k != loops.size() + 1; ++k)
    {
        // reset permutation for new value of k
        for (std::size_t i = 0; i != loops.size(); ++i)
            permutation[i] = i;

        /*
         * Graph determinant alternates between addition and subtraction of
         * non-touching loop combinations.
         */
        op::Op2 combineOperation = k % 2 ? op::sub : op::add;

        subPermutation.resize(k);
        bool loopsAreTouching;
        bool expressionWasModified = false;
        do
        {
            /*
             * For the current permutation, we need to go through all possible
             * pair combinations (k=2) to check if two loops are touching or
             * not.
             */
            for (std::size_t i = 0; i != k; ++i)
                subPermutation[i] = permutation[i];
            loopsAreTouching = false;
            do
            {
                if (touchingLoops[subPermutation[0]][subPermutation[1]-subPermutation[0]-1])
                {
                    loopsAreTouching = true;
                    break;
                }
            } while (next_combination(subPermutation.begin(), subPermutation.begin() + 2, subPermutation.end()));

            /*
             * If these loops are touching, then they should not be part of the
             * expression.
             */
            if (loopsAreTouching)
                continue;

            // Multiply all non-touching loop gains
            Expression* loopGain = loopExpressions[permutation[0]];
            for (std::size_t i = 1; i < k; ++i)
                loopGain = Expression::make(op::mul, loopGain, loopExpressions[permutation[i]]);

            // Finally, add (or subtract) to/from the final expression
            e = Expression::make(combineOperation, e, loopGain);
            expressionWasModified = true;

        } while (next_combination(permutation.begin(), permutation.begin() + k, permutation.end()));

        if (expressionWasModified == false)
            break;
    }

    return e;
}

// ----------------------------------------------------------------------------
Expression* Graph::calculateCofactorsAndPathGains(const PathList& paths, const PathList& loops) const
{
    if (paths.size() == 0)
        return Expression::make(0.0);

    Expression* e = NULL;
    for (PathList::const_iterator path = paths.begin(); path != paths.end(); ++path)
    {
        PathList nonTouchingLoops;
        for (PathList::const_iterator loop = loops.begin(); loop != loops.end(); ++loop)
        {
            if (pathsAreTouching(*path, *loop) == false)
                nonTouchingLoops.push_back(*loop);
        }

        Expression* pathGain = calculateConnectionGain(*path);
        if (nonTouchingLoops.size() > 0)
        {
            pathGain = Expression::make(op::mul, pathGain, calculateDeterminant(nonTouchingLoops));
        }

        if (e == NULL)
            e = pathGain;
        else
            e = Expression::make(op::add, e, pathGain);
    }

    return e;
}

// ----------------------------------------------------------------------------
bool Graph::pathsAreTouching(const Path& a, const Path& b) const
{
    for (Path::const_iterator ita = a.begin(); ita != a.end(); ++ita)
        for (Path::const_iterator itb = b.begin(); itb != b.end(); ++itb)
            if (ita->out_ == itb->out_ || ita->in_ == itb->in_)
                return true;
    return false;
}

// ----------------------------------------------------------------------------
void Graph::dump(const char* fileName) const
{
    FILE* fp = fopen(fileName, "w");
    dump(fp);
    fclose(fp);
}

// ----------------------------------------------------------------------------
void Graph::dump(FILE* fp) const
{

}

}
