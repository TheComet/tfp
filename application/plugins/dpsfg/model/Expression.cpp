#include "Expression.hpp"
#include <string.h>
#include <cassert>

using namespace dpsfg;

// ----------------------------------------------------------------------------
Expression::Expression() :
    parent_(NULL),
    left_(NULL),
    right_(NULL),
    operator_('\0')
{
}

// ----------------------------------------------------------------------------
Expression* Expression::asLHSOfSelf()
{
    Expression* inserted = new Expression;
    inserted->left_ = this;  // Need to do this here, so refcount of this doesn't drop to 0

    inserted->parent_ = parent_;
    if (parent_ != NULL)
    {
        assert(parent_->left_ == this || parent_->right_ == this);
        if (parent_->left_ == this)
            parent_->left_ = inserted;
        else
            parent_->right_ = inserted;
    }

    parent_ = inserted;  // now fix up parent

    return inserted;
}

// ----------------------------------------------------------------------------
Expression* Expression::newRHS()
{
    right_ = new Expression;
    right_->parent_ = this;
    return right_;
}
