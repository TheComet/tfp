#pragma once

#include "tfp/config.hpp"
#include "tfp/models/Reference.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include <QWidget>

namespace tfp {

class System;

class SystemVisualiser : public QWidget,
                         public SystemListener
{
    Q_OBJECT

public:
    explicit SystemVisualiser(QWidget* parent=NULL);
    ~SystemVisualiser();

    void setSystem(System* system);

    virtual void replot() = 0;
    virtual void autoScale() = 0;

protected:
    Reference<System> system_;
};

}
