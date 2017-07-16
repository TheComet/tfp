#pragma once

#include "tfp/config.hpp"

#include <QSlider>
#include <QStyle>
#include <QStyleOptionSlider>


namespace tfp {


class TFP_PUBLIC_API InfiniSliderWidget : public QSlider
{
    Q_OBJECT

public:
    explicit InfiniSliderWidget(QWidget* parent = 0);

    void setValue(double value);
    void setLogarithmic(bool flag);

signals:
    void valueChanged(double value);

protected:
    virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
    double value_;
    double scale_;
    bool isNegative_;
    bool isLogarithmic_;
};

} // namespace tfp
