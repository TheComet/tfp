#pragma once

#include "sfgsym/util/Reference.hpp"
#include <string>
#include <vector>
#include <memory>

namespace sfgsym {

class Connection;
class Graph;

class Node : public RefCounted
{
public:
    typedef std::vector< Reference<Connection> > ConnectionList;
    Node(Graph* owner, const std::string& name);

    /*!
     * Creates a new connection between this node and another node. The connection
     * is out-going (direction matters).
     */
    Connection* connectTo(Node* node);

    /*!
     * Creates a new connection between another node and this node. The connection
     * is incoming (direction matters).
     */
    Connection* connectFrom(Node* node);

    /*!
     * Disconnects this node from the specified node, regardless of connection
     * direction and regardless of how many connections exist between the two
     * nodes. (Destroys as many connections as necessary to disconnect these
     * nodes)
     */
    void disconnectFrom(Node* node);

    /*!
     * Disconnects all outgoing connections to the specified node. The other
     * node may still have incoming connections after this call returns.
     */
    void disconnectOutgoing(Node* node);

    const ConnectionList& outgoingConnections() const;

    std::string name() const;
    void setName(std::string name);

    std::string getPhysicalUnit() const;
    void setPhysicalUnit(std::string unit);

private:
    std::string name_;
    std::string physicalUnit_;
    ConnectionList outgoingConnections_;
    Graph* graph_;
};

}
