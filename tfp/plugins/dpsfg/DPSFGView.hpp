#pragma once

#include "tfp/config.hpp"
#include <QGraphicsView>

class DPSFGScene;

class DPSFGView : public QGraphicsView
{
  Q_OBJECT

public:
    explicit DPSFGView(DPSFGScene* scene);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void drawBackground(QPainter* painter, const QRectF& r) override;

private:
    void drawGrid(QPainter* painter, double gridStep);

private:
    DPSFGScene* scene_;
    QPointF lastClickPosition_;
};
