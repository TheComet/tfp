#pragma once

#include "tfp/config.hpp"
#include "tfp/math/TransferFunction.hpp"

namespace tfp {

class StateSpace
{
public:
    struct ABCD
    {
        ABCD() {}
        ABCD(RealMatrix A, RealMatrix B, RealMatrix C, RealMatrix D) :
            A(A), B(B), C(C), D(D) {}
        RealMatrix A;
        RealMatrix B;
        RealMatrix C;
        RealMatrix D;
    };

    StateSpace();
    StateSpace(const ABCD& abcd);

    StateSpace ccf() const;
    StateSpace ocf() const;
    TransferFunction tf() const;

private:
    ABCD abcd_;
};

}
