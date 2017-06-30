#pragma once

#include "tfp/config.hpp"
#include "tfp/views/DynamicSystemVisualiser.hpp"
#include "tfp/math/TransferFunction.hxx"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class TimeDomainResponsePlot : public DynamicSystemVisualiser
{
public:
    explicit TimeDomainResponsePlot(QWidget* parent=NULL);

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() = 0;

    virtual void replot() override;
    virtual void autoScale() override;
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    RealtimePlot* plot_;
    QwtPlotCurve* curve_;
};

}
