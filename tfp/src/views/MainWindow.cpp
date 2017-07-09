#include "tfp/ui_MainWindow.h"
#include "tfp/models/System.hpp"
#include "tfp/util/PluginManager.hpp"
#include "tfp/views/DataTree.hpp"
#include "tfp/views/MainWindow.hpp"
#include <QGridLayout>
#include <QMdiArea>
#include <QPainter>
#include <QSplitter>
#include <QDir>
#include <qwt_text.h>
#include <qwt_mathml_text_engine.h>

#include <dlfcn.h>

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
    mdiArea_(new QMdiArea),
    dataTree_(new DataTree),
    pluginManager_(new PluginManager(dataTree_))
{
    ui->setupUi(this);

    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));
    setWindowTitle("Transfer Function Plotter");

    // Need to explicitly enable MathML rendering support
    QwtText::setTextEngine(QwtText::MathMLText, new QwtMathMLTextEngine());

/*
    mdiArea_->setViewMode(QMdiArea::TabbedView);
    mdiArea_->setTabsClosable(true);
    mdiArea_->setTabsMovable(true);
    ui->centralWidget->layout()->addWidget(mdiArea_);
    mdiArea_->setVisible(false);*/

    loadPlugins();

    // TODO Just for debugging.

    QWidget* widget = new QWidget;

    QGridLayout* layout = new QGridLayout;
    widget->setLayout(layout);

    SystemManipulator* bodePlot = pluginManager_->createSystemManipulator("Bode Plot");
    SystemManipulator* pzplot = pluginManager_->createSystemManipulator("Pole Zero Plot");
    SystemManipulator* stepPlot = pluginManager_->createSystemManipulator("Step Response");
    SystemManipulator* pzplot3d = pluginManager_->createSystemManipulator("Complex Plane 3D");
    SystemManipulator* manipulator = pluginManager_->createSystemManipulator("Standard Low Order Filters");
    layout->addWidget(bodePlot, 0, 0, 2, 1);
    layout->addWidget(pzplot, 0, 1, 1, 1);
    layout->addWidget(stepPlot, 1, 1, 1, 1);
    layout->addWidget(pzplot3d, 0, 2, 1, 1);
    layout->addWidget(manipulator, 1, 2, 1, 1);

    System* system = newSystem("System");
    manipulator->setSystem(system);
    bodePlot->setSystem(system);
    pzplot->setSystem(system);
    pzplot3d->setSystem(system);
    stepPlot->setSystem(system);

    QSplitter* splitter = new QSplitter;
    ui->centralWidget->layout()->addWidget(splitter);
    splitter->addWidget(dataTree_);
    splitter->addWidget(widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ----------------------------------------------------------------------------
void MainWindow::loadPlugins()
{
    QDir pluginsDir("plugins");
    QStringList allFiles = pluginsDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    for (QStringList::const_iterator it = allFiles.begin(); it != allFiles.end(); ++it)
        pluginManager_->loadPlugin(QDir::toNativeSeparators("plugins/" + *it));
}

// ----------------------------------------------------------------------------
System* MainWindow::newSystem(const QString& name)
{
    System* system = new System(name);
    dataTree_->addSystem(system);
    return system;
}

// ----------------------------------------------------------------------------
void MainWindow::deleteSystem(System* system)
{
    dataTree_->removeSystem(system);
    delete system;
}

} // namespace tfp
