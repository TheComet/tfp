#include "ButterworthFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>

using namespace tfp;

// ----------------------------------------------------------------------------
ButterworthFilter::ButterworthFilter(QWidget* parent) :
    SystemManipulator(parent),
    n_(5), k_(1.0), wc_(1.0)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QWidget* nadj = new QWidget;
    nadj->setLayout(new QHBoxLayout);
    nadj->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(nadj);

    QLabel* nadjLabel = new QLabel;
    nadjLabel->setText("Order");
    nadj->layout()->addWidget(nadjLabel);

    QSpinBox* nadjSpinBox = new QSpinBox;
    nadjSpinBox->setRange(1, 100);
    nadjSpinBox->setValue(n_);
    nadj->layout()->addWidget(nadjSpinBox);

    FloatAdjustmentWidget* kadj = new FloatAdjustmentWidget;
    kadj->setName("k");
    kadj->setValue(k_);
    layout->addWidget(kadj);

    FloatAdjustmentWidget* wcadj = new FloatAdjustmentWidget;
    wcadj->setName("wc");
    wcadj->setValue(wc_);
    layout->addWidget(wcadj);

    //connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    //connect(wpadj, SIGNAL(valueChanged(double)), this, SLOT(setPoleFrequency(double)));
    //connect(qpadj_, SIGNAL(valueChanged(double)), this, SLOT(setQualityFactor(double)));
    //connect(filters, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterType(int)));
    connect(nadjSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setOrder(int)));
    connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(wcadj, SIGNAL(valueChanged(double)), this, SLOT(setCutoffFrequency(double)));
}

// ----------------------------------------------------------------------------
void ButterworthFilter::updateParameters()
{
    double factor = k_;
    for (int k = 0; k < n_; ++k)
    {
        system_->denominator_.setRoot(k, wc_ * std::exp(typename Type<double>::Complex(0, ((2*k+n_+1)*M_PI) / (2*n_))));
        factor *= wc_;
    }
    system_->numerator_.setFactor(factor);

    /* Chebyshev
    double eta = 1;
    double a = -std::sinh(1.0/n_ * std::asinh(1.0/eta));
    double b = std::cosh(1.0/n_ * std::asinh(1.0/eta));
    for (int i = 0; i < n_; ++i)
        denominator_.(i) = typename Type<double>::Complex(
            a * std::sin(M_PI / 2.0 * (2.0*i+1)/n_),
            b * std::cos(M_PI / 2.0 * (2.0*i+1)/n_)
        );*/

    system_->notifyParametersChanged();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setOrder(int n)
{
    n_ = n;
    system_->denominator_.resize(n_);
    system_->notifyStructureChanged();
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setScale(double k)
{
    k_ = k;
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::setCutoffFrequency(double wc)
{
    wc_ = wc;
    updateParameters();
}

// ----------------------------------------------------------------------------
void ButterworthFilter::onSetSystem()
{
    if (system_ == NULL)
        return;

    system_->numerator_.resize(0);
    setOrder(n_);
    updateParameters();
}
