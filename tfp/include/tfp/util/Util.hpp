#pragma once

#include "tfp/config.hpp"

class QLayout;
class QwtPlotTextLabel;

namespace tfp {

struct Util
{
    /*!
     * @brief Deletes all widgets, layouts, and spacer items from a layout,
     * effectively making it empty.
     */
    static void clearLayout(QLayout* layout);

    /*!
     * @brief Creates the "Loading..." text label for a Qwt plot. Since a few
     * parts of the program need these labels, the function for creating it
     * has been centralised here.
     */
    static QwtPlotTextLabel* createLoadingTextLabel();
};

} // namespace tfp
