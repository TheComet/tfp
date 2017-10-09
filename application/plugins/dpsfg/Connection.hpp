#pragma once

#include <QObject>

namespace tfp {

class Node;

class Connection : public QObject
{
    Q_OBJECT

public:
    void setExpression(const QString& expression);
    const QString& getExpression() const;
    void setTargetNode(Node* node);
    Node* getTargetNode() const;

signals:
    void expressionUpdated(Connection*);

private:
    QString  expression_;
    Node* outgoingNode_;
};

}
