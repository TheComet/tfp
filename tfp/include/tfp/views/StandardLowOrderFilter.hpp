#pragma once

#include "tfp/config.hpp"
#include "tfp/views/DynamicSystemConfig.hpp"

namespace tfp {

class FloatAdjustmentWidget;

class StandardLowOrderFilter : public DynamicSystemConfig
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

    virtual void getInterestingRange(double* xStart, double* xEnd) override;
    void setFilterType(FilterType filterType);

public slots:
    void setScale(double k);
    void setPoleFrequency(double wp);
    void setQualityFactor(double qp);
    void setParameters(double k, double wp, double qp);
    void setFilterType(int index);

private:
    FloatAdjustmentWidget* qpadj_;
    FilterType filterType_;
    double k_, wp_, qp_;
};

}
