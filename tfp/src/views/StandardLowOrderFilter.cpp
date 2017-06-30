#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QComboBox>

namespace tfp {

// ----------------------------------------------------------------------------
StandardLowOrderFilter::StandardLowOrderFilter(QWidget* parent) :
    DynamicSystemConfig(parent),
    qpadj_(new FloatAdjustmentWidget),
    filterType_(LOWPASS_1),
    k_(1.0), wp_(1.0), qp_(1.0)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QComboBox* filters = new QComboBox;
    filters->addItem("Lowpass 1st Order");
    filters->addItem("Highpass 1st Order");
    filters->addItem("Lowpass 2nd Order");
    filters->addItem("Bandpass");
    filters->addItem("Highpass 2nd Order");
    layout->addWidget(filters);

    FloatAdjustmentWidget* kadj = new FloatAdjustmentWidget;
    kadj->setName("k");
    kadj->setValue(1);
    layout->addWidget(kadj);

    FloatAdjustmentWidget* wpadj = new FloatAdjustmentWidget;
    wpadj->setName("wp");
    wpadj->setValue(1);
    layout->addWidget(wpadj);

    qpadj_->setName("qp");
    qpadj_->setValue(1);
    layout->addWidget(qpadj_);

    connect(kadj, SIGNAL(valueChanged(double)), this, SLOT(setScale(double)));
    connect(wpadj, SIGNAL(valueChanged(double)), this, SLOT(setPoleFrequency(double)));
    connect(qpadj_, SIGNAL(valueChanged(double)), this, SLOT(setQualityFactor(double)));
    connect(filters, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterType(int)));

    setFilterType(LOWPASS_1);
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::getInterestingRange(double* xStart, double* xEnd)
{
    *xStart = wp_ * 0.01;
    *xEnd = wp_ * 100;
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setParameters(double k, double wp, double qp)
{
    switch (filterType_)
    {
        case LOWPASS_1:
            /*
             *          k*wp
             * T(s) = --------
             *         s + wp
             */
            denominator_.roots_(0) = -wp_;
            break;

        case HIGHPASS_1:
            /*
             *          k*s
             * T(s) = --------
             *         s + wp
             */
            denominator_.roots_(0) = -wp_;
            break;

        // Denominator is calculated the same for all 3 variants
        case LOWPASS_2:
        case BANDPASS:
        case HIGHPASS_2:
        {
            /*
             *              k*wp^2
             * T(s) = --------------------
             *        s^2 + wp/qp*s + wp^2
             */

            // Real part
            denominator_.roots_(0).real(-wp_ / (2.0 * qp_));
            denominator_.roots_(1).real(-wp_ / (2.0 * qp_));

            // Roots are complex conjugates if qp > 0.5
            if (qp_ > 0.5)
            {
                denominator_.roots_(0).imag(+wp_ * std::sqrt(1.0 - 1.0/(4.0*qp_*qp_)));
                denominator_.roots_(1).imag(-wp_ * std::sqrt(1.0 - 1.0/(4.0*qp_*qp_)));
            }
            else
            {
                denominator_.roots_(0) += wp_ * std::sqrt(1.0/(4.0*qp_*qp_) - 1.0);
                denominator_.roots_(1) -= wp_ * std::sqrt(1.0/(4.0*qp_*qp_) - 1.0);
                denominator_.roots_(0).imag(0);
                denominator_.roots_(1).imag(0);
            }

            break;
        }

        default: break;
    }

    switch (filterType_)
    {
        case HIGHPASS_1:
        case HIGHPASS_2:
            numerator_.factor_ = k_;
            break;

        case LOWPASS_2:
            numerator_.factor_ = k_ * wp_ * wp_;
            break;

        case LOWPASS_1:
        case BANDPASS:
            numerator_.factor_ = k_ * wp_;
            break;

        default: break;
    }

    emit parametersChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setScale(double k)
{
    k_ = k;
    setParameters(k_, wp_, qp_);
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setPoleFrequency(double wp)
{
    wp_ = wp;
    setParameters(k_, wp_, qp_);
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setQualityFactor(double qp)
{
    qp_ = qp;
    setParameters(k_, wp_, qp_);
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(FilterType filterType)
{
    filterType_ = filterType;

    switch (filterType_)
    {
        case LOWPASS_1:
            qpadj_->setEnabled(false);
            numerator_.roots_.resize(0, Eigen::NoChange);
            denominator_.roots_.resize(1, Eigen::NoChange);
            break;

        case HIGHPASS_1:
            qpadj_->setEnabled(false);
            numerator_.roots_.resize(1, Eigen::NoChange);
            denominator_.roots_.resize(1, Eigen::NoChange);
            break;

        case LOWPASS_2:
            qpadj_->setEnabled(true);
            numerator_.roots_.resize(0, Eigen::NoChange);
            denominator_.roots_.resize(2, Eigen::NoChange);
            break;

        case BANDPASS:
            qpadj_->setEnabled(true);
            numerator_.roots_.resize(1, Eigen::NoChange);
            denominator_.roots_.resize(2, Eigen::NoChange);
            break;

        case HIGHPASS_2:
            qpadj_->setEnabled(true);
            numerator_.roots_.resize(2, Eigen::NoChange);
            denominator_.roots_.resize(2, Eigen::NoChange);
            break;

        default:
            std::cerr << "Invalid filter type " << filterType_ << std::endl;
            break;
    }
/*
    // Butterworth filter
    int N = 64;
    denominator_.roots_.resize(N, 1);
    for (int i = 0; i < N; ++i)
        denominator_.roots_(i) = std::pow(typename Type<double>::Complex(-1, 0), double(i+0.5)/(N)) * typename Type<double>::Complex(0, 1);

    // Chebyshev
    double eta = 1;
    double a = -std::sinh(1.0/N * std::asinh(1.0/eta));
    double b = std::cosh(1.0/N * std::asinh(1.0/eta));
    for (int i = 0; i < N; ++i)
        denominator_.roots_(i) = typename Type<double>::Complex(
            a * std::sin(M_PI / 2.0 * (2.0*i+1)/N),
            b * std::cos(M_PI / 2.0 * (2.0*i+1)/N)
        );*/

    emit structureChanged();
    setParameters(k_, wp_, qp_);
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(int index)
{
    setFilterType(FilterType(index));
}

}
