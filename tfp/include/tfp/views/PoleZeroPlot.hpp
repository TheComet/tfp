#pragma once

#include "tfp/config.hpp"
#include "tfp/views/DynamicSystemVisualiser.hpp"
#include <QVector>

class QwtPlotShapeItem;
class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class PoleZeroPlot : public DynamicSystemVisualiser
{
    Q_OBJECT

public:
    explicit PoleZeroPlot(QWidget* parent=NULL);

protected:
    virtual void autoScale() override;
    virtual void replot() override;
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    void onSystemChanged();

private:
    RealtimePlot* plot_;
    QVector<QwtPlotShapeItem*> poles_;
    QVector<QwtPlotShapeItem*> zeros_;
};

}
