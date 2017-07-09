#pragma once

#include "tfp/views/SystemVisualiser.hpp"

namespace Qwt3D {
    class GridPlot;
}

namespace tfp {

class ComplexPlane3D : public SystemVisualiser
{
public:
    explicit ComplexPlane3D(QWidget* parent=NULL);

    virtual void onSetSystem() override;
    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    void normaliseAxes();

private:
    Qwt3D::GridPlot* plot_;
};

}
