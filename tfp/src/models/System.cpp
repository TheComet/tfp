#include "tfp/models/System.hpp"
#include <QTreeWidgetItem>

namespace tfp {

// ----------------------------------------------------------------------------
System::System(QTreeWidgetItem* dataTree) :
    dataTree_(dataTree)
{
    tfItem_ = new QTreeWidgetItem(dataTree);
    tfItem_->setText(0, "Transfer Function");
    polesItem_ = new QTreeWidgetItem(tfItem_);
    polesItem_->setText(0, "Poles");
    zerosItem_ = new QTreeWidgetItem(tfItem_);
    zerosItem_->setText(0, "Zeros");
    coeffBItem_ = new QTreeWidgetItem(tfItem_);
    coeffBItem_->setText(0, "Numerator");
    coeffAItem_ = new QTreeWidgetItem(tfItem_);
    coeffAItem_->setText(0, "Denominator");
}

// ----------------------------------------------------------------------------
void System::notifyParametersChanged()
{
    CoefficientPolynomial<double> coeffNumerator = numerator_.poly();
    CoefficientPolynomial<double> coeffDenominator = denominator_.poly();
    QString buf;

#define UPDATE_CHILD_VALUES_COMPLEX(item, polynomial)                        \
    buf= "[";                                                                \
    for (int i = 0; i != polynomial.size(); ++i)                             \
    {                                                                        \
        QString num = QString::number(polynomial.root(i).real()) + "+" +     \
                      QString::number(polynomial.root(i).imag()) + "j";      \
        item->child(i)->setText(1, num);                                     \
        if (i > 0)                                                           \
            buf += ", ";                                                     \
        buf += num;                                                          \
    }                                                                        \
    buf += "]";                                                              \
    item->setText(1, buf);

#define UPDATE_CHILD_VALUES_REAL(item, polynomial)                           \
    buf= "[";                                                                \
    for (int i = 0; i != polynomial.size(); ++i)                             \
    {                                                                        \
        QString num = QString::number(polynomial.coefficient(i));            \
        item->child(i)->setText(1, num);                                     \
        if (i > 0)                                                           \
            buf += ", ";                                                     \
        buf += num;                                                          \
    }                                                                        \
    buf += "]";                                                              \
    item->setText(1, buf);

    UPDATE_CHILD_VALUES_COMPLEX(polesItem_, denominator_);
    UPDATE_CHILD_VALUES_COMPLEX(zerosItem_, numerator_);
    UPDATE_CHILD_VALUES_REAL(coeffBItem_, coeffNumerator);
    UPDATE_CHILD_VALUES_REAL(coeffAItem_, coeffDenominator);

    dispatcher.dispatch(&SystemListener::onSystemParametersChanged);
}


// ----------------------------------------------------------------------------
void System::notifyStructureChanged()
{
    CoefficientPolynomial<double> coeffNumerator = numerator_.poly();
    CoefficientPolynomial<double> coeffDenominator = denominator_.poly();
#define UPDATE_CHILD_COUNT(item, polynomial, name)                           \
    while (item->childCount() < polynomial.size())                           \
        item->addChild(new QTreeWidgetItem(QStringList(name + QString::number(item->childCount())))); \
    while (item->childCount() > polynomial.size())                           \
        delete item->takeChild(item->childCount() - 1);
    UPDATE_CHILD_COUNT(polesItem_, denominator_, "p");
    UPDATE_CHILD_COUNT(zerosItem_, numerator_, "z");
    UPDATE_CHILD_COUNT(coeffBItem_, coeffNumerator, "b");
    UPDATE_CHILD_COUNT(coeffAItem_, coeffDenominator, "a");

    dispatcher.dispatch(&SystemListener::onSystemStructureChanged);
}

// ----------------------------------------------------------------------------
void System::getInterestingFrequencyInterval(double* xStart, double* xEnd) const
{
    double minx = std::numeric_limits<double>::max();
    double maxx = 0;

    for (int i = 0; i != denominator_.size(); ++i)
    {
        double mag = std::abs(denominator_.root(i));
        if (maxx < mag)
            maxx = mag;
        if (minx > mag)
            minx = mag;
    }

    // In case of zero poles (because logarithmic plots depend on these ranges)
    if (minx == 0)
        minx = maxx;

    *xStart = minx * 0.01;
    *xEnd   = maxx * 100;
}

// ----------------------------------------------------------------------------
void System::getInterestingTimeInterval(double* xStart, double* xEnd) const
{
    double closestpole = std::numeric_limits<double>::max();

    for (int i = 0; i != denominator_.size(); ++i)
    {
        double real = std::abs(denominator_.root(i).real());
        if (closestpole > real)
            closestpole = real;
    }

    *xStart = 0;
    *xEnd   = 10 / closestpole;
}

// ----------------------------------------------------------------------------
QTreeWidgetItem* System::dataTree()
{
    return dataTree_;
}

}
