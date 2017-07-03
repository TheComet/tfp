#pragma once

#include "tfp/config.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include "tfp/math/TransferFunction.hpp"
#include "tfp/models/RefCounted.hpp"
#include "tfp/models/ListenerDispatcher.hpp"

namespace tfp {

class System : public RefCounted,
               public TransferFunction<double>
{
public:
    void notifyParametersChanged();
    void notifyStructureChanged();
    void getInterestingFrequencyInterval(double* xStart, double* xEnd) const;
    void getInterestingTimeInterval(double* xStart, double* xEnd) const;

    ListenerDispatcher<SystemListener> dispatcher;
};

}
