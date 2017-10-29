#include "tfp/ui_MainWindow.h"
#include "tfp/models/System.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/util/Util.hpp"
#include "tfp/views/DataTree.hpp"
#include "tfp/views/MainWindow.hpp"
#include "tfp/views/SettingsView.hpp"
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
    virtual void paintEvent(QPaintEvent* e) OVERRIDE
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
    tools1_(new QComboBox),
    tools2_(new QComboBox),
    toolContainer1_(new QWidget),
    toolContainer2_(new QWidget),
    pluginManager_(new PluginManager(dataTree_))
{
    ui->setupUi(this);
    
    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));
    setWindowTitle(qtTrId(ID_APP_TITLE));

/*
    mdiArea_->setViewMode(QMdiArea::TabbedView);
    mdiArea_->setTabsClosable(true);
    mdiArea_->setTabsMovable(true);
    ui->centralWidget->layout()->addWidget(mdiArea_);
    mdiArea_->setVisible(false);*/

    loadPlugins();
    tools1_ = new QComboBox;
    tools2_ = new QComboBox;
    QVector<ToolFactory*> toolsList = pluginManager_->getToolsList();
    for (QVector<ToolFactory*>::const_iterator it = toolsList.begin(); it != toolsList.end(); ++it)
    {
        tools1_->addItem((*it)->name);
        tools2_->addItem((*it)->name);
    }

    QWidget* leftPane = new QWidget;
    leftPane->setLayout(new QVBoxLayout);
    leftPane->layout()->addWidget(dataTree_);
    leftPane->layout()->addWidget(tools1_);
    leftPane->layout()->addWidget(tools2_);

    activeSystem_ = newSystem("System");
    toolContainer1_->setLayout(new QHBoxLayout);
    toolContainer2_->setLayout(new QHBoxLayout);

    QSplitter* splitter = new QSplitter;
    ui->centralWidget->layout()->addWidget(splitter);
    splitter->addWidget(leftPane);
    splitter->addWidget(toolContainer1_);
    splitter->addWidget(toolContainer2_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    connect(tools1_, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(loadTool1(const QString&)));
    connect(tools2_, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(loadTool2(const QString&)));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onActionQuitTriggered()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onActionSettingsTriggered()));

    loadTool1("DPSFG");
    loadTool2("Bode Plot");
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ----------------------------------------------------------------------------
void MainWindow::loadPlugins()
{
    pluginManager_->loadAllPlugins();

    std::cout << "Available tools:" << std::endl;
    QVector<ToolFactory*> toolsList = pluginManager_->getToolsList();
    for (QVector<ToolFactory*>::const_iterator it = toolsList.begin(); it != toolsList.end(); ++it)
    {
        std::cout << "  + "
            << (*it)->name.toStdString() << " ("
            << (*it)->blurb.toStdString() << ") -- "
            << (*it)->author.toStdString()
            << std::endl;
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

// ----------------------------------------------------------------------------
void MainWindow::loadTool1(const QString& name)
{
    Tool* tool = pluginManager_->createTool(name);
    if (tool == NULL)
        return;

    // Update combo box
    for (int i = 0; i < tools1_->count(); ++i)
        if (tools1_->itemText(i) == name)
        {
            bool blocked = blockSignals(true);
            tools1_->setCurrentIndex(i);
            blockSignals(blocked);
            break;
        }

    Util::clearLayout(toolContainer1_->layout());
    tool->setSystem(activeSystem_);
    toolContainer1_->layout()->addWidget(tool);
}
void MainWindow::loadTool2(const QString& name)
{
    Tool* tool = pluginManager_->createTool(name);
    if (tool == NULL)
        return;

    // Update combo box
    for (int i = 0; i < tools2_->count(); ++i)
        if (tools2_->itemText(i) == name)
        {
            bool blocked = blockSignals(true);
            tools2_->setCurrentIndex(i);
            blockSignals(blocked);
            break;
        }

    Util::clearLayout(toolContainer2_->layout());
    tool->setSystem(activeSystem_);
    toolContainer2_->layout()->addWidget(tool);
}

// ----------------------------------------------------------------------------
void MainWindow::onActionQuitTriggered()
{
    close();
}

// ----------------------------------------------------------------------------
void MainWindow::onActionSettingsTriggered()
{
    SettingsView* settingsView = new SettingsView;
    settingsView->setAttribute(Qt::WA_DeleteOnClose);
    settingsView->show();
}

} // namespace tfp
