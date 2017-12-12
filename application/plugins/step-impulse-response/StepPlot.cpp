#include "StepPlot.hpp"
#include "tfp/math/System.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
StepPlot::StepPlot(QWidget* parent) :
    TimeDomainResponsePlot(parent)
{
}

// ----------------------------------------------------------------------------
TransferFunction::PFEResultData StepPlot::doPartialFractionExpansion()
{
    return system_->tf().partialFractionExpansion(1);
}
