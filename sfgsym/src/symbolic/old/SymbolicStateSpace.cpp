#include "tfp/math/SymbolicStateSpace.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
SymbolicStateSpace::SymbolicStateSpace()
{
}

// ----------------------------------------------------------------------------
SymbolicStateSpace::SymbolicStateSpace(const ABCD& abcd) :
    abcd_(abcd)
{
}

// ----------------------------------------------------------------------------
SymbolicStateSpace SymbolicStateSpace::ccf() const
{
    return SymbolicStateSpace();
}

// ----------------------------------------------------------------------------
SymbolicStateSpace SymbolicStateSpace::ocf() const
{
    return SymbolicStateSpace();
}

// ----------------------------------------------------------------------------
StateSpace SymbolicStateSpace::ss(const VariableTable* vt) const
{
    return StateSpace();
}

// ----------------------------------------------------------------------------
SymbolicTransferFunction SymbolicStateSpace::tf() const
{
    return SymbolicTransferFunction();
}
