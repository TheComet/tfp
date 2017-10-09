#pragma once

#include "tfp/util/RefCounted.hpp"
#include <QVector>

namespace dpsfg {

class Connection;

class Node : public tfp::RefCounted
{
public:
    Node();
    Node(const QString& name);

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

    const QVector<Connection*>& getOutgoingConnections() const;

    QString getName() const;
    void setName(QString name);

    QString getPhysicalUnit() const;
    void setPhysicalUnit(QString unit);

private:
    QString name_;
    QString physicalUnit_;
    QVector<Connection*> outgoingConnections_;
};

}
