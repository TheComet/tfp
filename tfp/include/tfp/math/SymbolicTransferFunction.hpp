#pragma once

#include "tfp/config.hpp"
#include "tfp/math/Expression.hpp"
#include <vector>

namespace tfp {

class SymbolicTransferFunction
{
public:
    typedef std::vector< Reference<Expression> > ExpressionList;


private:
    ExpressionList numerator_;
    ExpressionList denominator_;
};

}
