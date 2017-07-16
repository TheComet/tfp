#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class QMdiArea;
class QComboBox;

namespace tfp {

class DataTree;
class PluginManager;
class System;

/*!
 * @brief The top-most Qt widget of this application. Handles the menu bar
 * actions and sets up global stuff (like shortcuts, icons, etc.)
 */
class TFP_PUBLIC_API MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! Constructor
    explicit MainWindow(QWidget* parent=NULL);
    //! Default destructor
    ~MainWindow();

    System* newSystem(const QString& name);
    void deleteSystem(System* system);

public slots:
    void loadTool1(const QString& name);
    void loadTool2(const QString& name);

private:
    void loadPlugins();

private:
    Ui::MainWindow* ui;
    QMdiArea* mdiArea_;
    DataTree* dataTree_;
    QComboBox* tools1_;
    QComboBox* tools2_;
    QWidget* toolContainer1_;
    QWidget* toolContainer2_;
    Reference<PluginManager> pluginManager_;
    Reference<System> activeSystem_;
};

} // namespace tfp
