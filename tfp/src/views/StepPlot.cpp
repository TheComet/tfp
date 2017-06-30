#include "tfp/views/StepPlot.hpp"
#include "tfp/views/DynamicSystemConfig.hpp"
#include "tfp/math/TransferFunction.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
StepPlot::StepPlot(QWidget* parent) :
    TimeDomainResponsePlot(parent)
{
}

// ----------------------------------------------------------------------------
TransferFunction<double>::PFEResultData StepPlot::doPartialFractionExpansion()
{
    return system_->partialFractionExpansion(1);
}

}
