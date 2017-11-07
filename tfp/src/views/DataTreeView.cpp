#include "tfp/views/DataTreeView.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
DataTreeView::DataTreeView(QWidget* parent) :
    QTreeWidget(parent)
{
    setColumnCount(2);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //headerItem()->setHidden(true);
}

// ----------------------------------------------------------------------------
void DataTreeView::addSystem(System* system)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    addTopLevelItem(item);
    item->setExpanded(true);
    item->setText(0, system->name());
    system->setDataTree(item);
}

// ----------------------------------------------------------------------------
void DataTreeView::removeSystem(System* system)
{
    delete takeTopLevelItem(indexOfTopLevelItem(system->dataTree()));
}

}
