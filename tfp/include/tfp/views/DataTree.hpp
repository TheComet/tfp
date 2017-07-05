#pragma once

#include "tfp/config.hpp"
#include <QTreeWidget>

namespace tfp {

class DataTree : public QTreeWidget
{
public:
    explicit DataTree(QWidget* parent=NULL);

    QTreeWidgetItem* addSystem(const QString& name);
    void removeSystem(QTreeWidgetItem* system);

private:
    int getTopLevelItemIndex(QTreeWidgetItem* item);
};

}
