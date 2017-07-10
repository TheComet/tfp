#pragma once

#include "tfp/views/SystemManipulator.hpp"

namespace tfp {

class FloatAdjustmentWidget;

class StandardLowOrderFilter : public SystemManipulator
{
    Q_OBJECT

public:
    enum FilterType
    {
        LOWPASS_1 = 0,
        HIGHPASS_1,
        LOWPASS_2,
        BANDPASS,
        HIGHPASS_2,
        FILTER_COUNT
    };

    explicit StandardLowOrderFilter(QWidget* parent=NULL);

    void setFilterType(FilterType filterType);

public slots:
    void setScale(double k);
    void setPoleFrequency(double wp);
    void setQualityFactor(double qp);
    void setFilterType(int index);

protected:
    virtual void onSetSystem() override;

private:
    void updateParameters();

private:
    FloatAdjustmentWidget* qpadj_;
    FilterType filterType_;
    double k_, wp_, qp_;
};

}
