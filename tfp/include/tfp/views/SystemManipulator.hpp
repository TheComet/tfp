#pragma once

#include "tfp/config.hpp"
#include "tfp/models/Reference.hpp"
#include <QWidget>

namespace tfp {

class System;

/*!
 * @brief Represents a continuous dynamic system.
 *
 * The numerator and denominator objects can be publicly accessed (numerator_
 * and denominator_). When doing so, you need to manually emit either
 * parametersChanged() (if you only modified existing values) or
 * structureChanged() (if you added or removed roots).
 */
class SystemManipulator : public QWidget
{
public:
    explicit SystemManipulator(QWidget* parent=NULL);
    void setSystem(System* system);

protected:
    virtual void onSetSystem() = 0;

protected:
    Reference<System> system_;
};

}
