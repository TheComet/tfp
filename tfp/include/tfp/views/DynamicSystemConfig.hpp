#pragma once

#include "tfp/config.hpp"
#include "tfp/math/TransferFunction.hxx"
#include <QWidget>

namespace tfp {

/*!
 * @brief Represents a continuous dynamic system.
 *
 * The numerator and denominator objects can be publicly accessed (numerator_
 * and denominator_). When doing so, you need to manually emit either
 * parametersChanged() (if you only modified existing values) or
 * structureChanged() (if you added or removed roots).
 */
class DynamicSystemConfig : public QWidget,
                            public TransferFunction<double>
{
    Q_OBJECT

public:
    explicit DynamicSystemConfig(QWidget* parent=NULL);

    virtual void getInterestingRange(double* xStart, double* xEnd) = 0;

signals:
    void parametersChanged();
    void structureChanged();
};

}
