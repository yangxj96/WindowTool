#ifndef WINTOOL_MAIN_WINDOW_H
#define WINTOOL_MAIN_WINDOW_H

#include <QMainWindow>

#include "ServiceWidget.h"

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

    ServiceWidget *service_widget;
};


#endif //WINTOOL_MAIN_WINDOW_H