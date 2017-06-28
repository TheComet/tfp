#pragma once

#include <QMainWindow>

#include "tfp/config.hpp"

namespace Ui {
    class MainWindow;
}

namespace tfp {

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

private:
    Ui::MainWindow* ui;
};

} // namespace tfp
