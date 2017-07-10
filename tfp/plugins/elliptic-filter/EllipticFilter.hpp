#pragma once

#include "tfp/views/SystemManipulator.hpp"

class QSpinBox;

namespace tfp {

class FloatAdjustmentWidget;

class EllipticFilter : public SystemManipulator
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

private:
    void updateParameters();

private:
    double n_, k_, wc_, epsilon_;
};

}
