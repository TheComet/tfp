#include "NodeGraphicsObject.hpp"
#include <QPainter>

// ----------------------------------------------------------------------------
NodeGraphicsObject::NodeGraphicsObject(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
}

// ----------------------------------------------------------------------------
void NodeGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
}

// ----------------------------------------------------------------------------
QRectF NodeGraphicsObject::boundingRect() const
{
    return QRectF(-10, -10, 20, 20);
}
