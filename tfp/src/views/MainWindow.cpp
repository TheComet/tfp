#include "tfp/ui_MainWindow.h"
#include "tfp/views/MainWindow.hpp"
#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/views/BodePlot.hpp"
#include "tfp/views/PoleZeroPlot.hpp"
#include "tfp/views/StepPlot.hpp"
#include "tfp/views/ImpulsePlot.hpp"
#include <QGridLayout>
#include <qwt_text.h>
#include <qwt_mathml_text_engine.h>

namespace tfp {

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));

    // Need to explicitly enable MathML rendering support
    QwtText::setTextEngine(QwtText::MathMLText, new QwtMathMLTextEngine());

    QWidget* widget = new QWidget;
    centralWidget()->layout()->addWidget(widget);
    QGridLayout* layout = new QGridLayout;
    widget->setLayout(layout);

    DynamicSystemVisualiser* bodePlot = new BodePlot;
    DynamicSystemVisualiser* pzplot = new PoleZeroPlot;
    DynamicSystemVisualiser* stepPlot = new StepPlot;
    layout->addWidget(bodePlot, 0, 0, 2, 1);
    layout->addWidget(pzplot, 0, 1, 1, 1);
    layout->addWidget(stepPlot, 1, 1, 1, 1);

    StandardLowOrderFilter* filterConfig = new StandardLowOrderFilter;
    layout->addWidget(filterConfig, 0, 2, 2, 1);

    bodePlot->setSystem(filterConfig);
    pzplot->setSystem(filterConfig);
    stepPlot->setSystem(filterConfig);
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace tfp
