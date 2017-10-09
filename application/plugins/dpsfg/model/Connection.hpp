#pragma once

#include "tfp/util/RefCounted.hpp"
#include <QString>

namespace dpsfg {

class Node;

class Connection : public tfp::RefCounted
{
public:
    void setExpression(const QString& expression) { expression_ = expression; }
    const QString& getExpression() const { return expression_; }
    void setTargetNode(Node* node) { outgoingNode_ = node; }
    Node* getTargetNode() const { return outgoingNode_; }

private:
    QString expression_;
    Node*   outgoingNode_;
};

}
