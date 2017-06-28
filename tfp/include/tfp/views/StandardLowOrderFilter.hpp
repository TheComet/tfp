#pragma once

#include "tfp/config.hpp"
#include "tfp/math/TransferFunction.hxx"
#include <QWidget>

namespace tfp {

class FloatAdjustmentWidget;

class StandardLowOrderFilter : public QWidget,
                               public TransferFunction<double>
{
    Q_OBJECT

public:
    explicit StandardLowOrderFilter(QWidget* parent=NULL);

    enum FilterType
    {
        LOWPASS_1 = 0,
        HIGHPASS_1,
        LOWPASS_2,
        BANDPASS,
        HIGHPASS_2,
        FILTER_COUNT
    };

    void setFilterType(FilterType filterType);

signals:
    void valuesChanged();
    void structureChanged();

public slots:
    void setScale(double k);
    void setPoleFrequency(double wp);
    void setQualityFactor(double qp);
    void setFilterType(int index);

private:
    FloatAdjustmentWidget* qpadj_;
    FilterType filterType_;
};

}
