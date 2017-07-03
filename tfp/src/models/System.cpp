#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
void System::notifyParametersChanged()
{
    dispatcher.dispatch(&SystemListener::onSystemParametersChanged);
}


// ----------------------------------------------------------------------------
void System::notifyStructureChanged()
{
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

}
