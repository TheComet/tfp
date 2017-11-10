#include "tfp/math/Connection.hpp"
#include "tfp/math/Node.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
Node::Node()
{
}

// ----------------------------------------------------------------------------
Node::Node(const std::string& name) :
    name_(name)
{
}

// ----------------------------------------------------------------------------
Connection* Node::connectTo(Node* other)
{
    outgoingConnections_.emplace_back(new Connection);
    outgoingConnections_.back()->setTargetNode(other);
    return outgoingConnections_.back().get();
}

// ----------------------------------------------------------------------------
Connection* Node::connectFrom(Node* other)
{
    return other->connectTo(this);
}

// ----------------------------------------------------------------------------
void Node::disconnectFrom(Node* other)
{
    disconnectOutgoing(other);
    other->disconnectOutgoing(this);
}

// ----------------------------------------------------------------------------
void Node::disconnectOutgoing(Node* other)
{
    // Take care of all outgoing connections
    for (ConnectionList::iterator it = outgoingConnections_.begin(); it != outgoingConnections_.end();)
    {
        if ((*it)->targetNode() == other)
            it = outgoingConnections_.erase(it);
        else
            ++it;
    }
}

// ----------------------------------------------------------------------------
const Node::ConnectionList& Node::outgoingConnections() const
{
    return outgoingConnections_;
}

// ----------------------------------------------------------------------------
std::string Node::getName() const
{
    return name_;
}

// ----------------------------------------------------------------------------
void Node::setName(std::string name)
{
    name_ = name;
}

// ----------------------------------------------------------------------------
std::string Node::getPhysicalUnit() const
{
    return physicalUnit_;
}

// ----------------------------------------------------------------------------
void Node::setPhysicalUnit(std::string unit)
{
    physicalUnit_ = unit;
}

