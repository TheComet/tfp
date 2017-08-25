#pragma once

namespace tfp {

class Connection;

class Node
{
public:
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
	 * direction.
	 */
	void disconnectFrom(Node* node);

	const QVector<Connection*>& getOutgoingConnections() const;

	QString getPhysicalUnit() const;
	void setPhysicalUnit(const QString& unit);

private:
	QVector<Connection*> outgoingConnections_;
	QString physicalUnit_;
};

}
