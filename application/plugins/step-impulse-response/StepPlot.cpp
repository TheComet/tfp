#include "StepPlot.hpp"
#include "tfp/math/System.hpp"

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
