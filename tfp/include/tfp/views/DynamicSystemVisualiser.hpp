#pragma once

#include "tfp/config.hpp"
#include <QWidget>

namespace tfp {

class DynamicSystemConfig;

class DynamicSystemVisualiser : public QWidget
{
    Q_OBJECT

public:
    explicit DynamicSystemVisualiser(QWidget* parent=NULL);

    void setSystem(DynamicSystemConfig* system);

    virtual void replot() = 0;
    virtual void autoScale() = 0;

protected:
    virtual void onSystemParametersChanged() = 0;
    virtual void onSystemStructureChanged() = 0;

private slots:
    void onParametersChanged();
    void onStructureChanged();

protected:
    DynamicSystemConfig* system_;
};

}
