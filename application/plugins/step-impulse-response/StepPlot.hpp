#pragma once

#include "TimeDomainResponsePlot.hpp"

class StepPlot : public TimeDomainResponsePlot
{
public:
    explicit StepPlot(QWidget* parent=NULL);

protected:
    virtual tfp::TransferFunction::PFEResultData doPartialFractionExpansion() OVERRIDE;
};
