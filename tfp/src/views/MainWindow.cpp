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
    numerator(0) = 10;
    denominator(0) = 2;
    denominator(1) = 1;
    denominator(2) = 2;
    TransferFunction<double> T(numerator, denominator);

    Type<double>::RealArray step = T.stepResponse(0, 50, 100);

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
