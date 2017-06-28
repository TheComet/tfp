#pragma once

#include "tfp/config.hpp"
#include <QWidget>

class QwtPlotCurve;

namespace tfp {

class StandardLowOrderFilter;
class RealtimePlot;

class BodePlot : public QWidget
{
    Q_OBJECT

public:
    explicit BodePlot(QWidget* parent=NULL);

    void setTransferFunction(StandardLowOrderFilter* tf);
    void replot();
    void autoScale();

private slots:
    void onTFChanged();

private:
    StandardLowOrderFilter* tf_;
    RealtimePlot* ampPlot_;
    RealtimePlot* phasePlot_;
    QwtPlotCurve* amplitude_;
    QwtPlotCurve* phase_;
};

}
