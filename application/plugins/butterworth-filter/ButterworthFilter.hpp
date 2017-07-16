#pragma once

#include "tfp/plugin/Tool.hpp"

class QSpinBox;

namespace tfp {

class FloatAdjustmentWidget;

class ButterworthFilter : public Tool
{
    Q_OBJECT

public:
    explicit ButterworthFilter(QWidget* parent=NULL);

public slots:
    void setOrder(int n);
    void setScale(double k);
    void setCutoffFrequency(double wc);

protected:
    virtual void onSetSystem() override;
    virtual void onSystemStructureChanged() override {}
    virtual void onSystemParametersChanged() override {}
    virtual void replot() override {}
    virtual void autoScale() override {}

private:
    void updateParameters();

private:
    double n_, k_, wc_;
};

}
