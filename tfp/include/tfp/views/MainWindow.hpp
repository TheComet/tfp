#pragma once

#include <QMainWindow>
#include <QMap>

#include "tfp/config.hpp"


class QMdiArea;
class QCloseEvent;
class QSignalMapper;


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
    explicit MainWindow(QWidget* parent = 0);
    //! Default destructor
    ~MainWindow();

private:
    Ui::MainWindow* ui;
};

} // namespace tfp
