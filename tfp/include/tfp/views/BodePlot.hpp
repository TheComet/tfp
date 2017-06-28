#pragma once

#include "tfp/config.hpp"
#include "tfp/views/DynamicSystemVisualiser.hpp"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class BodePlot : public DynamicSystemVisualiser
{
    Q_OBJECT

public:
    explicit BodePlot(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    void onSystemChanged();

protected:
    RealtimePlot* ampPlot_;
    RealtimePlot* phasePlot_;
    QwtPlotCurve* amplitude_;
    QwtPlotCurve* phase_;
};

}
