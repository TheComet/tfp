#pragma once

#include "tfp/views/Tool.hpp"

class QSpinBox;

namespace tfp {

class FloatAdjustmentWidget;

class EllipticFilter : public Tool
{
    Q_OBJECT

public:
    explicit EllipticFilter(QWidget* parent=NULL);

public slots:
    void setOrder(int n);
    void setScale(double k);
    void setCutoffFrequency(double wc);
    void setEpsilon(double epsilon);

protected:
    virtual void onSetSystem() override;
    virtual void onSystemStructureChanged() override {}
    virtual void onSystemParametersChanged() override {}
    virtual void replot() override {}
    virtual void autoScale() override {}

private:
    void updateParameters();

private:
    double n_, k_, wc_, epsilon_;
};

}
