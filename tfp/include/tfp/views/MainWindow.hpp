#pragma once

#include <QMainWindow>

#include "tfp/config.hpp"

namespace Ui {
    class MainWindow;
}

class QMdiArea;

namespace tfp {

class System;
class DataTree;

/*!
 * @brief The top-most Qt widget of this application. Handles the menu bar
 * actions and sets up global stuff (like shortcuts, icons, etc.)
 */
class MainWindow : public QMainWindow
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
    Ui::MainWindow* ui;
    QMdiArea* mdiArea_;
    DataTree* dataTree_;
};

} // namespace tfp
