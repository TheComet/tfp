#pragma once

#include "tfp/plugin/Tool.hpp"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class BodePlot : public Tool
{
public:
    explicit BodePlot(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSetSystem() override {}
    virtual void onSystemStructureChanged() override;
    virtual void onSystemParametersChanged() override;

protected:
    RealtimePlot* ampPlot_;
    RealtimePlot* phasePlot_;
    QwtPlotCurve* amplitude_;
    QwtPlotCurve* phase_;
};

}
