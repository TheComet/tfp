#include "ImpulsePlot.hpp"
#include "tfp/math/System.hpp"

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
