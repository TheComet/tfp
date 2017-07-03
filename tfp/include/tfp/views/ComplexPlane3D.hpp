#pragma once


#include "tfp/config.hpp"
#include "tfp/views/SystemVisualiser.hpp"

namespace Qwt3D {
    class GridPlot;
}

namespace tfp {

class ComplexPlane3D : public SystemVisualiser
{
public:
    explicit ComplexPlane3D(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    Qwt3D::GridPlot* plot_;
};

}
