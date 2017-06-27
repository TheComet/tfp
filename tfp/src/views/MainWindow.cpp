#include "tfp/ui_MainWindow.h"
#include "tfp/views/MainWindow.hpp"
#include "tfp/models/TransferFunction.hpp"

#include <iostream>
#include <cmath>

namespace tfp {

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));

    Eigen::Matrix<double, Eigen::Dynamic, 1> x(1, 1);
    x(0, 0) = 4;

    CoefficientPolynomial<double> numerator(1);
    CoefficientPolynomial<double> denominator(3);
    numerator(0) = 1;
    denominator(0) = 1;
    denominator(1) = 0.5;
    denominator(2) = 1;
    TransferFunction<double> T(numerator, denominator);

    Type<double>::ComplexArray t = T.frequencyResponse(0.01, 100, 100);
    Type<double>::RealArray impulse = T.impulseResponse(0, 50, 100);
    Type<double>::RealArray step = T.stepResponse(0, 50, 100);

    std::cout << "[" << std::abs(t(0));
    for (int i = 1; i < t.size(); ++i)
        std::cout << ", " << std::abs(t(i));
    std::cout << "];" << std::endl;
    std::cout << "[" << std::arg(t(0));
    for (int i = 1; i < t.size(); ++i)
        std::cout << ", " << std::arg(t(i));
    std::cout << "];" << std::endl;

    std::cout << "[" << std::arg(impulse(0));
    for (int i = 1; i < impulse.size(); ++i)
        std::cout << ", " << impulse(i);
    std::cout << "];" << std::endl;

    std::cout << "[" << std::arg(step(0));
    for (int i = 1; i < step.size(); ++i)
        std::cout << ", " << step(i);
    std::cout << "];" << std::endl;
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace tfp
