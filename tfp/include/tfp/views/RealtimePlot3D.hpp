#pragma once

#include <qwt3d_gridplot.h>

namespace Qwt3D {
    class Function;
}

namespace tfp {

class RealtimePlot3D : public Qwt3D::GridPlot
{
public:
    explicit RealtimePlot3D(QWidget* parent=NULL);
    void efficientlyUpdateData(Qwt3D::Function* function);
};

}
