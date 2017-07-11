#pragma once

#include "tfp/views/Tool.hpp"
#include <QVector>

class QwtPlotShapeItem;
class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class PoleZeroPlot : public Tool
{
public:
    explicit PoleZeroPlot(QWidget* parent=NULL);

    virtual void autoScale() override;
    virtual void replot() override;

protected:
    virtual void onSetSystem() override {}
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
