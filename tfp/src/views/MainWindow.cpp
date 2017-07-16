#include "tfp/ui_MainWindow.h"
#include "tfp/models/System.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/util/Util.hpp"
#include "tfp/views/DataTree.hpp"
#include "tfp/views/MainWindow.hpp"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMdiArea>
#include <QGroupBox>
#include <QComboBox>
#include <QPainter>
#include <QSplitter>
#include <QDir>
#include <qwt_text.h>

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

/*
    mdiArea_->setViewMode(QMdiArea::TabbedView);
    mdiArea_->setTabsClosable(true);
    mdiArea_->setTabsMovable(true);
    ui->centralWidget->layout()->addWidget(mdiArea_);
    mdiArea_->setVisible(false);*/
    
    loadPlugins();

    System* system = newSystem("System");
    QWidget* widget = new QWidget;
    widget->setLayout(new QHBoxLayout);
    
    Tool* dpsfg = pluginManager_->createTool("DPSFG");
    dpsfg->setSystem(system);
    widget->layout()->addWidget(dpsfg);

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

    QStringList validExtensions;
    validExtensions += ".dll";
    validExtensions += ".so";
    validExtensions += ".dylib";
    validExtensions += ".dynlib";

    for (QStringList::const_iterator it = allFiles.begin(); it != allFiles.end(); ++it)
        for (QStringList::const_iterator ext = validExtensions.begin(); ext != validExtensions.end(); ++ext)
            if (it->endsWith(*ext, Qt::CaseInsensitive))
            {
                pluginManager_->loadPlugin(QDir::toNativeSeparators("plugins/" + *it));
                break;
            }
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
