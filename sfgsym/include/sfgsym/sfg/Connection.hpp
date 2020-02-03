#pragma once

#include "sfgsym/util/Reference.hpp"

namespace sfgsym {

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

    void setTargetNode(Node* node);
    Node* targetNode() const;

    void reset();
    Type type() const;

    void setExpression(Expression* expression);
    Expression* expression() const;

    void setSystem(System* system);
    System* system() const;

private:
    Node* targetNode_;
    union {
        Expression* expression_;
        System*     system_;
    } data_;
    Type type_;
};

}
