#pragma once

#include "tfp/views/SystemManipulator.hpp"

class QSpinBox;

namespace tfp {

class FloatAdjustmentWidget;

class ButterworthFilter : public SystemManipulator
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

private:
    void updateParameters();

private:
    double n_, k_, wc_;
};

}
