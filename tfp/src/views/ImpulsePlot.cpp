#include "tfp/views/ImpulsePlot.hpp"
#include "tfp/views/DynamicSystemConfig.hpp"
#include "tfp/math/TransferFunction.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
ImpulsePlot::ImpulsePlot(QWidget* parent) :
    TimeDomainResponsePlot(parent)
{
}

// ----------------------------------------------------------------------------
TransferFunction<double>::PFEResultData ImpulsePlot::doPartialFractionExpansion()
{
    return system_->partialFractionExpansion(0);
}

}
