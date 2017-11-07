#pragma once

#include "tfp/config.hpp"
#include <QTreeWidget>

namespace tfp {

class System;

class TFP_PUBLIC_API DataTreeView : public QTreeWidget
{
public:
    explicit DataTreeView(QWidget* parent=NULL);

    void addSystem(System* system);
    void removeSystem(System* system);

private:
    int getTopLevelItemIndex(QTreeWidgetItem* item);
};

}
