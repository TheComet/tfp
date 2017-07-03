#include "tfp/ui_MainWindow.h"
#include "tfp/views/MainWindow.hpp"
#include "tfp/views/StandardLowOrderFilter.hpp"
#include "tfp/views/BodePlot.hpp"
#include "tfp/views/PoleZeroPlot.hpp"
#include "tfp/views/StepPlot.hpp"
#include "tfp/views/ImpulsePlot.hpp"
#include "tfp/views/ComplexPlane3D.hpp"
#include <QGridLayout>
#include <QMdiArea>
#include <QPainter>
#include <QPushButton>
#include <qwt_text.h>
#include <qwt_mathml_text_engine.h>

namespace tfp {

class MdiAreaWithBackgroundImage : public QMdiArea
{
public:
    explicit MdiAreaWithBackgroundImage(QWidget* parent = 0) : QMdiArea(parent)
    {
        setBackground(QBrush(QColor(190, 190, 190)));
    }
protected:
    virtual void paintEvent(QPaintEvent* e) override
    {
        QMdiArea::paintEvent(e);

        QPainter painter(viewport());
        QPixmap pixmap = QPixmap(":/background/welcome.png").scaled(size(), Qt::KeepAspectRatio);
        painter.drawPixmap((size().width() - pixmap.width()) / 2, 0, pixmap);
    }
};

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mdiArea_(new QMdiArea)
{
    ui->setupUi(this);

    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));
    setWindowTitle("Transfer Function Plotter");

    // Need to explicitly enable MathML rendering support
    QwtText::setTextEngine(QwtText::MathMLText, new QwtMathMLTextEngine());

    mdiArea_->setViewMode(QMdiArea::TabbedView);
    mdiArea_->setTabsClosable(true);
    mdiArea_->setTabsMovable(true);
    ui->centralWidget->layout()->addWidget(mdiArea_);

    // TODO Just for debugging.

    QWidget* widget = new QWidget;
    mdiArea_->addSubWindow(widget);
    widget->showMaximized();

    QGridLayout* layout = new QGridLayout;
    widget->setLayout(layout);

    SystemVisualiser* bodePlot = new BodePlot;
    layout->addWidget(bodePlot, 0, 0, 2, 1);
    SystemVisualiser* pzplot = new PoleZeroPlot;
    layout->addWidget(pzplot, 0, 1, 1, 1);
    SystemVisualiser* stepPlot = new StepPlot;
    layout->addWidget(stepPlot, 1, 1, 1, 1);
    SystemVisualiser* pzplot3d = new ComplexPlane3D;
    layout->addWidget(pzplot3d, 0, 2, 1, 1);

    SystemManipulator* manipulator = new StandardLowOrderFilter;
    layout->addWidget(manipulator, 1, 2, 1, 1);

    System* system = manipulator->createSystem();
    manipulator->setSystem(system);
    bodePlot->setSystem(system);
    pzplot->setSystem(system);
    pzplot3d->setSystem(system);
    stepPlot->setSystem(system);

}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace tfp
