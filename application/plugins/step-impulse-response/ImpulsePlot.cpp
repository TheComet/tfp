#include "ImpulsePlot.hpp"
#include "tfp/math/System.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
ImpulsePlot::ImpulsePlot(QWidget* parent) :
    TimeDomainResponsePlot(parent)
{
}

// ----------------------------------------------------------------------------
tfp::TransferFunction::PFEResultData ImpulsePlot::doPartialFractionExpansion()
{
    return system_->tf().partialFractionExpansion(0);
}
