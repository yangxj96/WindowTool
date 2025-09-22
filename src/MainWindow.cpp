#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->service_widget = new ServiceWidget(this);

    ui->tw->addTab(service_widget, tr("服务管理"));
}

MainWindow::~MainWindow() {
    delete ui;
}
