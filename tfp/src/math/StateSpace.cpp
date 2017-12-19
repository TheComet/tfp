#include "tfp/math/StateSpace.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
StateSpace::StateSpace()
{
}

// ----------------------------------------------------------------------------
StateSpace::StateSpace(const ABCD& abcd) :
    abcd_(abcd)
{
}

// ----------------------------------------------------------------------------
StateSpace StateSpace::ccf() const
{
    return StateSpace();
}

// ----------------------------------------------------------------------------
StateSpace StateSpace::ocf() const
{
    return StateSpace();
}

// ----------------------------------------------------------------------------
TransferFunction StateSpace::tf() const
{
    return TransferFunction();
}
