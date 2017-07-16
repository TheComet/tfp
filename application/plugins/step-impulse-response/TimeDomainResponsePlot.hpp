#pragma once

#include "tfp/config.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/math/TransferFunction.hxx"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class TimeDomainResponsePlot : public Tool
{
public:
    explicit TimeDomainResponsePlot(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual TransferFunction<double>::PFEResultData doPartialFractionExpansion() = 0;
    virtual void onSetSystem() override {}
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    RealtimePlot* plot_;
    QwtPlotCurve* curve_;
};

}
