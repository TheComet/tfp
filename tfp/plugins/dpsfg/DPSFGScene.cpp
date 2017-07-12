#include "DPSFGScene.hpp"
#include "NodeGraphicsObject.hpp"

// ----------------------------------------------------------------------------
DPSFGScene::DPSFGScene(QObject* parent) :
    QGraphicsScene(parent)
{
    addText("Hello World");
    addItem(new NodeGraphicsObject);
}
