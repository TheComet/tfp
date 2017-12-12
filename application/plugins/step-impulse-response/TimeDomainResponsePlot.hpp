#pragma once

#include "tfp/config.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/math/TransferFunction.hpp"

class QwtPlotCurve;

namespace tfp {
    class RealtimePlot;
}

class TimeDomainResponsePlot : public tfp::Tool
{
public:
    explicit TimeDomainResponsePlot(QWidget* parent=NULL);

    virtual void replot() OVERRIDE;
    virtual void autoScale() OVERRIDE;

protected:
    virtual tfp::TransferFunction::PFEResultData doPartialFractionExpansion() = 0;
    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE;
    virtual void onSystemStructureChanged() OVERRIDE;

private:
    tfp::RealtimePlot* plot_;
    QwtPlotCurve* curve_;
};
