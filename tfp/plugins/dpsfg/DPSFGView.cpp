#include "DPSFGView.hpp"
#include "DPSFGScene.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

// ----------------------------------------------------------------------------
DPSFGView::DPSFGView(DPSFGScene* scene) :
    QGraphicsView(scene),
    scene_(scene)
{
}

// ----------------------------------------------------------------------------
void DPSFGView::mousePressEvent(QMouseEvent* event)
{
    //QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::MiddleButton)
        lastClickPosition_ = mapToScene(event->pos());
}

// ----------------------------------------------------------------------------
void DPSFGView::mouseMoveEvent(QMouseEvent* event)
{
    //QGraphicsView::mouseMoveEvent(event);
    if (scene_->mouseGrabberItem() == NULL && event->buttons() == Qt::MiddleButton)
    {
        QPointF diff = lastClickPosition_ - mapToScene(event->pos());
        setSceneRect(sceneRect().translated(diff.x(), diff.y()));
    }
}

// ----------------------------------------------------------------------------
void DPSFGView::drawBackground(QPainter* painter, const QRectF& r)
{
    const unsigned char light = 230;
    const unsigned char dark = 210;

    QPen pen;
    pen.setWidth(1.0);
    pen.setColor(QColor(light, light, light));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    drawGrid(painter, 15);

    pen.setColor(QColor(dark, dark, dark));
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    drawGrid(painter, 150);
}

// ----------------------------------------------------------------------------
void DPSFGView::drawGrid(QPainter* painter, double gridStep)
{
    QRect windowRect = rect();
    QPointF topLeft = mapToScene(windowRect.topLeft());
    QPointF bottomRight = mapToScene(windowRect.bottomRight());

    double left   = std::floor(topLeft.x()     / gridStep - 0.5);
    double right  = std::floor(bottomRight.x() / gridStep + 1.0);
    double bottom = std::floor(topLeft.y()     / gridStep - 0.5);
    double top    = std::floor(bottomRight.y() / gridStep + 1.0);

    // Vertical lines
    for (int i = int(left); i < int(right); ++i)
    {
        QLineF line(i * gridStep, bottom * gridStep, i * gridStep, top * gridStep);
        painter->drawLine(line);
    }

    // Horizontal lines
    for (int i = int(bottom); i < int(top); ++i)
    {
        QLineF line(left * gridStep, i * gridStep, right * gridStep, i * gridStep);
        painter->drawLine(line);
    }
}
