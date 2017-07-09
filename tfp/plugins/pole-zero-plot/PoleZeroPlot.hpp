#pragma once

#include "tfp/views/SystemVisualiser.hpp"
#include <QVector>

class QwtPlotShapeItem;
class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class PoleZeroPlot : public SystemVisualiser
{
public:
    explicit PoleZeroPlot(QWidget* parent=NULL);

protected:
    virtual void onSetSystem() override;
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
