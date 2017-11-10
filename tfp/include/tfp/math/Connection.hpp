#pragma once

#include "tfp/util/Reference.hpp"
#include <QString>

namespace tfp {

class Expression;
class Node;
class System;

class Connection : public RefCounted
{
public:
    enum Type
    {
        INVALID,
        EXPRESSION,
        SYSTEM
    };

    Connection();
    ~Connection();

    void reset();

    Type type() const;

    void setExpression(Expression* expression);
    Expression* expression() const;

    void setSystem(System* system);
    System* system() const;

    void setTargetNode(Node* node);
    Node* targetNode() const;

private:
    Node* outgoingNode_;
    union {
        Expression* expression_;
        System*     system_;
    } data_;
    Type type_;
};

}
