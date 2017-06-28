#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/views/FloatAdjustmentWidget.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVBoxLayout>
#include <QComboBox>

namespace tfp {

// ----------------------------------------------------------------------------
StandardLowOrderFilter::StandardLowOrderFilter(QWidget* parent) :
    QWidget(parent),
    qpadj_(new FloatAdjustmentWidget),
    filterType_(LOWPASS_1)
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
void StandardLowOrderFilter::setScale(double k)
{
    switch (filterType_)
    {
        case LOWPASS_1:
            break;

        case HIGHPASS_1:
            break;

        case LOWPASS_2:
            break;

        case BANDPASS:
            break;

        case HIGHPASS_2:
            break;

        default: break;
    }

    emit valuesChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setPoleFrequency(double wp)
{
    switch (filterType_)
    {
        case LOWPASS_1:
            break;

        case HIGHPASS_1:
            break;

        case LOWPASS_2:
            break;

        case BANDPASS:
            break;

        case HIGHPASS_2:
            break;

        default: break;
    }

    emit valuesChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setQualityFactor(double qp)
{
    switch (filterType_)
    {
        case LOWPASS_1:
            break;

        case HIGHPASS_1:
            break;

        case LOWPASS_2:
            break;

        case BANDPASS:
            break;

        case HIGHPASS_2:
            break;

        default: break;
    }

    emit valuesChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(FilterType filterType)
{
    switch (filterType)
    {
        case LOWPASS_1:
            qpadj_->setEnabled(false);

            // Zeros
            numerator_.roots_.resize(0, Eigen::NoChange);
            numerator_.factor_ = 1;

            // Poles
            denominator_.roots_.resize(1, Eigen::NoChange);
            denominator_.roots_ << -1;
            denominator_.factor_ = 1;
            break;

        case HIGHPASS_1:
            qpadj_->setEnabled(false);

            // Zeros
            numerator_.roots_.resize(1, Eigen::NoChange);
            numerator_.roots_ << 0;
            numerator_.factor_ = 1;

            // Poles
            denominator_.roots_.resize(1, Eigen::NoChange);
            denominator_.roots_ << -1;
            denominator_.factor_ = 1;
            break;

        case LOWPASS_2:
            qpadj_->setEnabled(true);

            // Zeros
            numerator_.roots_.resize(0, Eigen::NoChange);
            numerator_.factor_ = 1;

            // Poles
            denominator_.roots_.resize(2, Eigen::NoChange);
            denominator_.roots_ << std::complex<double>(-0.25, 0.968245836551854),
                                   std::complex<double>(-0.25, -0.968245836551854);
            denominator_.factor_ = 1;

            break;

        case BANDPASS:
            qpadj_->setEnabled(true);

            // Zeros
            numerator_.roots_.resize(1, Eigen::NoChange);
            numerator_.roots_ << 0;
            numerator_.factor_ = 1;

            // Poles
            denominator_.roots_.resize(2, Eigen::NoChange);
            denominator_.roots_ << std::complex<double>(-0.25, 0.968245836551854),
                                   std::complex<double>(-0.25, -0.968245836551854);
            denominator_.factor_ = 1;
            break;

        case HIGHPASS_2:
            qpadj_->setEnabled(true);

            // Zeros
            numerator_.roots_.resize(2, Eigen::NoChange);
            numerator_.roots_ << 0, 0;
            numerator_.factor_ = 1;

            // Poles
            denominator_.roots_.resize(2, Eigen::NoChange);
            denominator_.roots_ << std::complex<double>(-0.25, 0.968245836551854),
                                   std::complex<double>(-0.25, -0.968245836551854);
            denominator_.factor_ = 1;
            break;

        default:
            std::cerr << "Invalid filter type " << filterType << std::endl;
            break;
    }

    emit structureChanged();
}

// ----------------------------------------------------------------------------
void StandardLowOrderFilter::setFilterType(int index)
{
    setFilterType(FilterType(index));
}

}
