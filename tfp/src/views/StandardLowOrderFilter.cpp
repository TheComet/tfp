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
void StandardLowOrderFilter::setScale(double k)
{
    k_ = k;

    switch (filterType_)
    {
        case LOWPASS_1:
            /*
             *          k*wp
             * T(s) = --------
             *         s + wp
             */
            numerator_.factor_ = k_ * wp_;
            break;

        case HIGHPASS_1:
            /*
             *          k*s
             * T(s) = --------
             *         s + wp
             */
            numerator_.factor_ = k_;
            break;

        case LOWPASS_2:
            /*
             *              k*wp^2
             * T(s) = --------------------
             *        s^2 + wp/qp*s + wp^2
             */
            numerator_.factor_ = k_ * wp_ * wp_;
            break;

        case BANDPASS:
            /*
             *              k*wp*s
             * T(s) = --------------------
             *        s^2 + wp/qp*s + wp^2
             */
            numerator_.factor_ = k_ * wp_;
            break;

        case HIGHPASS_2:
            /*
             *              k*s^2
             * T(s) = --------------------
             *        s^2 + wp/qp*s + wp^2
             */
            numerator_.factor_ = k_;
            break;

        default: break;
    }

    emit parametersChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setPoleFrequency(double wp)
{
    wp_ = wp;

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

    // Pole frequency affects numerator with these filter types
    switch (filterType_)
    {
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
void StandardLowOrderFilter::setQualityFactor(double qp)
{
    qp_ = qp;

    switch (filterType_)
    {
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

    emit parametersChanged();
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

    setScale(k_);
    setPoleFrequency(wp_);
    setQualityFactor(qp_);

    emit structureChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(int index)
{
    setFilterType(FilterType(index));
}

}
