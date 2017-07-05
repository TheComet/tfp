#include "tfp/views/DataTree.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
DataTree::DataTree(QWidget* parent) :
    QTreeWidget(parent)
{
    setColumnCount(2);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //headerItem()->setHidden(true);
}

// ----------------------------------------------------------------------------
QTreeWidgetItem* DataTree::addSystem(const QString& name)
{
    QTreeWidgetItem* system = new QTreeWidgetItem;
    system->setText(0, name);
    addTopLevelItem(system);
    return system;
}

// ----------------------------------------------------------------------------
void DataTree::removeSystem(QTreeWidgetItem* system)
{
    delete takeTopLevelItem(indexOfTopLevelItem(system));
}

}
