#ifndef WINTOOL_MAIN_WINDOW_H
#define WINTOOL_MAIN_WINDOW_H

#include <QMainWindow>

#include "service_widget.h"
#include "jdks_widget.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow* ui;

    ServiceWidget* widget_service;
    JdksWidget* widget_jdks;
};


#endif //WINTOOL_MAIN_WINDOW_H
