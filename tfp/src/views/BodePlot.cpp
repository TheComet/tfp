#include "tfp/views/BodePlot.hpp"
#include "tfp/views/RealtimePlot.hpp"
#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>

namespace tfp {

// ----------------------------------------------------------------------------
BodePlot::BodePlot(QWidget* parent) :
    QWidget(parent),
    tf_(NULL),
    ampPlot_(new RealtimePlot),
    phasePlot_(new RealtimePlot),
    amplitude_(new QwtPlotCurve),
    phase_(new QwtPlotCurve)
{
    setLayout(new QVBoxLayout);

    ampPlot_->enableRectangleZoom();
    ampPlot_->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
    layout()->addWidget(ampPlot_);

    phasePlot_->enableRectangleZoom();
    phasePlot_->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
    layout()->addWidget(phasePlot_);

    amplitude_->setTitle("Amplitude");
    amplitude_->attach(ampPlot_);

    phase_->setTitle("Phase");
    phase_->attach(phasePlot_);
}

// ----------------------------------------------------------------------------
void BodePlot::setTransferFunction(StandardLowOrderFilter* tf)
{
    if (tf_ != NULL)
    {
        disconnect(tf_, SIGNAL(valuesChanged()), this, SLOT(onTFChanged()));
        disconnect(tf_, SIGNAL(structureChanged()), this, SLOT(onTFChanged()));
    }

    tf_ = tf;

    if (tf_ != NULL)
    {
        connect(tf_, SIGNAL(valuesChanged()), this, SLOT(onTFChanged()));
        connect(tf_, SIGNAL(structureChanged()), this, SLOT(onTFChanged()));
        onTFChanged();
        autoScale();
    }
}

// ----------------------------------------------------------------------------
void BodePlot::autoScale()
{
    ampPlot_->autoScale();
    phasePlot_->autoScale();
}

// ----------------------------------------------------------------------------
void BodePlot::replot()
{
    ampPlot_->replot();
    phasePlot_->replot();
}

// ----------------------------------------------------------------------------
void BodePlot::onTFChanged()
{
    double ampdata[1000];
    double phasedata[1000];
    double freqdata[1000];

    tf_->frequencyResponse(freqdata, ampdata, phasedata, 0.01, 100, 1000);
    amplitude_->setSamples(freqdata, ampdata, 1000);
    phase_->setSamples(freqdata, phasedata, 1000);

    replot();
    autoScale();
}

}
