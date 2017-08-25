#pragma once

#include <QObject>

namespace tfp {

class Connection : public QObject
{
	Q_OBJECT

public:
	void setExpression(const QString& expression);
	const QString& getExpression() const;
	void setOutgoingNode(Node* node);
	Node* getOutgoingNode() const;

signals:
	void expressionUpdated(Connection*);

private:
	QString  expression_;
	Node* outgoingNode_;
};

}
