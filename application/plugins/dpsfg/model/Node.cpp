#include "model/Connection.hpp"
#include "model/Node.hpp"

using namespace dpsfg;

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
    Connection* c = new Connection;
    c->setTargetNode(other);
    outgoingConnections_.push_back(c);
    return c;
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
    for (std::vector< tfp::Reference<Connection> >::iterator it = outgoingConnections_.begin(); it != outgoingConnections_.end();)
    {
        if ((*it)->getTargetNode() == other)
            it = outgoingConnections_.erase(it);
        else
            ++it;
    }
}

// ----------------------------------------------------------------------------
const std::vector< tfp::Reference<Connection> >& Node::getOutgoingConnections() const
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

