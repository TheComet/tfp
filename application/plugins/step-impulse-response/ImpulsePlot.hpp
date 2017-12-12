#pragma once

#include "TimeDomainResponsePlot.hpp"

class ImpulsePlot : public TimeDomainResponsePlot
{
public:
    explicit ImpulsePlot(QWidget* parent=NULL);

protected:
    virtual tfp::TransferFunction::PFEResultData doPartialFractionExpansion() OVERRIDE;
};
