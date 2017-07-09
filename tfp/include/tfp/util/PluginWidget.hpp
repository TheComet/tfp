#pragma once

#include "tfp/config.hpp"
#include <QWidget>

namespace tfp {

class PluginWidget : public QWidget
{
public:
    explicit PluginWidget(QWidget* parent=NULL);
    ~PluginWidget();

private:
    QWidget* pluginWidget_;
};

}
