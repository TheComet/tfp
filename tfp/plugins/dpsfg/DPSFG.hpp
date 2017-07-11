#pragma once

#include "tfp/config.hpp"
#include "tfp/views/Tool.hpp"

class QwtPlotCurve;

namespace tfp {

class RealtimePlot;

class DPSFG : public Tool
{
public:
    explicit DPSFG(QWidget* parent=NULL);

protected:
    virtual void onSetSystem() override;
    virtual void onSystemStructureChanged() override {}
    virtual void onSystemParametersChanged() override {}
    virtual void replot() override {}
    virtual void autoScale() override {}

private:
};

}
