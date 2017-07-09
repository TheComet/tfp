#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include "tfp/views/SystemManipulator.hpp"

namespace tfp {

class System;

class SystemVisualiser : public SystemManipulator,
                         public SystemListener
{
public:
    explicit SystemVisualiser(QWidget* parent=NULL);
    ~SystemVisualiser();
    virtual void setSystem(System* system) override;

    virtual void replot() = 0;
    virtual void autoScale() = 0;
};

}
