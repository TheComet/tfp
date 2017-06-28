#include "tfp/ui_MainWindow.h"
#include "tfp/views/MainWindow.hpp"
#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/views/BodePlot.hpp"

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

    QLayout* layout = centralWidget()->layout();

    BodePlot* filterVisualiser = new BodePlot;
    layout->addWidget(filterVisualiser);

    StandardLowOrderFilter* filterConfig = new StandardLowOrderFilter;
    layout->addWidget(filterConfig);

    filterVisualiser->setTransferFunction(filterConfig);
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace tfp
