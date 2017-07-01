#pragma once


#include "tfp/config.hpp"
#include "tfp/views/DynamicSystemVisualiser.hpp"

namespace Qwt3D {
    class SurfacePlot;
}

namespace tfp {

class ComplexPlane3D : public DynamicSystemVisualiser
{
public:
    explicit ComplexPlane3D(QWidget* parent=NULL);

    virtual void replot() override;
    virtual void autoScale() override;

protected:
    virtual void onSystemParametersChanged() override;
    virtual void onSystemStructureChanged() override;

private:
    Qwt3D::SurfacePlot* plot_;
};

}
