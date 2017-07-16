#pragma once

#include "tfp/config.hpp"
#include <QGraphicsObject>

class NodeGraphicsObject : public QGraphicsObject
{
public:
    explicit NodeGraphicsObject(QGraphicsItem* parent=NULL);

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=NULL) override;
    virtual QRectF boundingRect() const override;
};
