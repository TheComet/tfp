#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class QMdiArea;
class QGroupBox;

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

private:
    void loadPlugins();

private:
    Ui::MainWindow* ui;
    QMdiArea* mdiArea_;
    DataTree* dataTree_;
    Reference<PluginManager> pluginManager_;
};

} // namespace tfp
