#pragma once

#include "tfp/config.hpp"
#include "tfp/views/TimeDomainResponsePlot.hpp"

namespace tfp {

class StepPlot : public TimeDomainResponsePlot
{
public:
    explicit StepPlot(QWidget* parent=NULL);

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() override;
};

}
