#include "model/Connection.hpp"
#include "model/Graph.hpp"
#include "model/Node.hpp"
#include "model/NChooseK.hpp"

using namespace dpsfg;

// ----------------------------------------------------------------------------
void Graph::setForwardPath(Node* in, Node* out)
{
    input_ = in;
    output_ = out;
}

// ----------------------------------------------------------------------------
tfp::TransferFunction<double> Graph::calculateTransferFunction() const
{
    return tfp::TransferFunction<double>();
}

// ----------------------------------------------------------------------------
bool Graph::evaluatePhysicalUnitConsistencies() const
{
    return false;
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoops(PathList* paths, PathList* loops) const
{
    findForwardPathsAndLoopsRecursive(paths, loops, input_, NodeList());
}

// ----------------------------------------------------------------------------
void Graph::findForwardPathsAndLoopsRecursive(PathList* paths, PathList* loops,
                                              Node* current, NodeList list) const
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
void Graph::nodeListToPath(Path* path, const NodeList& nodes) const
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
Expression* Graph::mason() const
{
    PathList paths;
    PathList loops;
    findForwardPathsAndLoops(&paths, &loops);

    Expression* graphDeterminant = calculateGraphDeterminant(loops);

    std::vector< std::vector<bool> > touchingPaths(paths.size());
    for (std::size_t inner = 0; inner != paths.size(); ++inner)
    {
        std::vector<bool> touching(loops.size());
        for (std::size_t outer = 0; outer != loops.size(); ++outer)
        {
            touching.push_back(pathsAreTouching(paths[inner], loops[outer]));
        }
        touchingPaths.push_back(touching);
    }

    return Expression::make(1.0);
}

// ----------------------------------------------------------------------------
Expression* Graph::calculatePathExpression(const Path& path) const
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

// ----------------------------------------------------------------------------
Expression* Graph::calculateGraphDeterminant(const PathList& loops) const
{
    // Nothing to do if there are no loops
    if (loops.size() == 0)
        return Expression::make(1.0);

    /*
     * Multiply all gain factors in each loop now, because these expressions
     * could potentially be used thousands of times.
     */
    std::vector< tfp::Reference<Expression> > loopExpressions(loops.size());
    for (std::size_t i = 0; i != loops.size(); ++i)
        loopExpressions[i] = calculatePathExpression(loops[i]);

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
    std::vector< std::vector<bool> > touchingLoops(loops.size());
    for (std::size_t i = 0; i != loops.size(); ++i)
        for (std::size_t j = i+1; j < loops.size(); ++j)
            touchingLoops[i].push_back(pathsAreTouching(loops[i], loops[j]));

    std::vector<int> permutation(loops.size());
    std::vector<int> subPermutation(loops.size());
    std::size_t k = 1;  // We begin at k=2
    while (true)
    {
        // reset permutation for new value of k
        k++;
        for (std::size_t i = 0; i != loops.size(); ++i)
            permutation[i] = i;
        subPermutation.resize(k);

        /*
         * Graph determinant alternates between addition and subtraction of
         * non-touching loops.
         */
        op::Op2 combineOperation = k % 2 ? op::sub : op::add;

        do
        {
            /*
             * For the current permutation, we need to go through all possible
             * pair combinations (k=2) to check if two loops are touching or
             * not.
             */
            for (std::size_t i = 0; i != k; ++i)
                subPermutation[i] = permutation[i];
            bool loopsAreTouching = false;
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
            Expression* loopExp = loopExpressions[permutation[0]];
            for (std::size_t i = 1; i < k; ++i)
                loopExp = Expression::make(op::mul, loopExp, loopExpressions[permutation[i]]);

            // Finally, add (or subtract) to/from the final expression
            e = Expression::make(combineOperation, e, loopExp);

        } while (next_combination(permutation.begin(), permutation.begin() + k, permutation.end()));
    }

    return e;
}

// ----------------------------------------------------------------------------
bool Graph::pathsAreTouching(const Path& a, const Path& b) const
{
    for (Path::const_iterator ita = a.begin(); ita != a.end(); ++ita)
        for (Path::const_iterator itb = b.begin(); itb != b.end(); ++itb)
            if (*ita == *itb)
                return true;
    return false;
}
