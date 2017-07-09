#pragma once

#include "tfp/views/SystemVisualiser.hpp"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class BodePlot : public SystemVisualiser
{
public:
    explicit BodePlot(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSetSystem() override;
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

protected:
    RealtimePlot* ampPlot_;
    RealtimePlot* phasePlot_;
    QwtPlotCurve* amplitude_;
    QwtPlotCurve* phase_;
};

}
