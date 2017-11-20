#include "tfp/math/Connection.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/math/System.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
Connection::Connection() :
    type_(INVALID)
{
}

// ----------------------------------------------------------------------------
Connection::~Connection()
{
    reset();
}

// ----------------------------------------------------------------------------
void Connection::setTargetNode(Node* node)
{
    targetNode_ = node;
}

// ----------------------------------------------------------------------------
Node* Connection::targetNode() const
{
    return targetNode_;
}

// ----------------------------------------------------------------------------
Connection::Type Connection::type() const
{
    return type_;
}

// ----------------------------------------------------------------------------
void Connection::reset()
{
    if (type() == SYSTEM)
        data_.system_->releaseRef();
    if (type() == EXPRESSION)
        data_.expression_->releaseRef();
}

// ----------------------------------------------------------------------------
void Connection::setExpression(Expression* expression)
{
    reset();

    data_.expression_ = expression;
    data_.expression_->addRef();
    type_ = EXPRESSION;
}

// ----------------------------------------------------------------------------
Expression* Connection::expression() const
{
    assert (type() == EXPRESSION);
    return data_.expression_;
}

// ----------------------------------------------------------------------------
void Connection::setSystem(System* system)
{
    reset();

    data_.system_ = system;
    data_.system_->addRef();
    type_ = SYSTEM;
}

// ----------------------------------------------------------------------------
System* Connection::system() const
{
    assert (type() == SYSTEM);
    return data_.system_;
}
