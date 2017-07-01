#include "tfp/views/ComplexPlane3D.hpp"
#include "tfp/views/DynamicSystemConfig.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <qwt3d_surfaceplot.h>
#include <qwt3d_function.h>
#include <qwt3d_color.h>

namespace tfp {

class Jet : public Qwt3D::StandardColor
{
public:
    explicit Jet(Qwt3D::Plot3D* data, unsigned size = 100) : Qwt3D::StandardColor(data, size) {}
    void reset(unsigned size)
    {
        colors_ = Qwt3D::ColorVector(size);
        Qwt3D::RGBA elem;
     
        double dsize = size;
     
        for (unsigned int i=0; i!=size; ++i)
        {
            elem.r = red(i/dsize);
            elem.g = green(i/dsize);
            elem.b = blue(i/dsize);
            elem.a = 1.0;
            colors_[i] = elem;
        }
    }

    double interpolate( double val, double y0, double x0, double y1, double x1 ) {
        return (val-x0)*(y1-y0)/(x1-x0) + y0;
    }

    double base( double val ) {
        if ( val <= -0.75 ) return 0;
        else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
        else if ( val <= 0.25 ) return 1.0;
        else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
        else return 0.0;
    }

    double red( double gray ) {
        return base( gray - 0.5 );
    }
    double green( double gray ) {
        return base( gray );
    }
    double blue( double gray ) {
        return base( gray + 0.5 );
    }
};

class MagnitudeFunction : public Qwt3D::Function
{
public:
    MagnitudeFunction(Qwt3D::SurfacePlot* pw, TransferFunction<double>* tf) :
        Function(*pw),
        tf_(tf)
    {
    }

    double operator()(double x, double y)
    {
        return std::log10(20 * std::abs(tf_->evaluate(typename Type<double>::Complex(x, y))));
    }

private:
    TransferFunction<double>* tf_;
};

// ----------------------------------------------------------------------------
ComplexPlane3D::ComplexPlane3D(QWidget* parent) :
    DynamicSystemVisualiser(parent),
    plot_(new Qwt3D::SurfacePlot)
{
    setLayout(new QVBoxLayout);

    plot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot_->resize(200, 200);

    for (unsigned i=0; i!=plot_->coordinates()->axes.size(); ++i)
    {
        plot_->coordinates()->axes[i].setMajors(5);
        plot_->coordinates()->axes[i].setMinors(5);
    }

    // TODO why the fuck isn't this working
    plot_->coordinates()->axes[Qwt3D::X1].setLabelString("Current (A)");
    plot_->coordinates()->axes[Qwt3D::Y1].setLabelString("Exposure (%)");
    plot_->coordinates()->axes[Qwt3D::Z1].setLabelString("Voltage (V)");
    plot_->coordinates()->axes[Qwt3D::X2].setLabelString("Current (A)");
    plot_->coordinates()->axes[Qwt3D::Y2].setLabelString("Exposure (%)");
    plot_->coordinates()->axes[Qwt3D::Z2].setLabelString("Voltage (V)");
    plot_->coordinates()->axes[Qwt3D::X3].setLabelString("Current (A)");
    plot_->coordinates()->axes[Qwt3D::Y3].setLabelString("Exposure (%)");
    plot_->coordinates()->axes[Qwt3D::Z3].setLabelString("Voltage (V)");

    plot_->setRotation(30, 0, 15);
    plot_->setScale(1, 1, 1);
    plot_->setShift(0.15, 0, 0);
    plot_->setZoom(0.9);
    plot_->setPlotStyle(Qwt3D::FILLED);
    //plot_->setDataColor(new Jet(plot_));
    plot_->setCoordinateStyle(Qwt3D::BOX);
    plot_->setOrtho(false);

    layout()->addWidget(plot_);
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::replot()
{
    plot_->updateData();
    plot_->updateGL();
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::autoScale()
{

}

// ----------------------------------------------------------------------------
void ComplexPlane3D::onSystemParametersChanged()
{
    MagnitudeFunction func(plot_, system_);
    
    func.setMesh(100, 100);
    func.setDomain(-5.5, 0.5, -3, 3);
    func.setMinZ(-5);
    func.setMaxZ(5);
    func.create();

    replot();
}

// ----------------------------------------------------------------------------
void ComplexPlane3D::onSystemStructureChanged()
{
    MagnitudeFunction func(plot_, system_);

    func.setMesh(100, 100);
    func.setDomain(-5.5, 0.5, -3, 3);
    func.setMinZ(-5);
    func.setMaxZ(5);
    func.create();
}

}
