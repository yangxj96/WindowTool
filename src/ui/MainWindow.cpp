#include "../../include/WinTool/ui/MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->widget_service = new ServiceWidget(this);
    this->widget_jdks = new JdksWidget(this);

    ui->tw->addTab(widget_service, tr("服务管理"));
    ui->tw->addTab(widget_jdks, tr("JDK管理"));
}

MainWindow::~MainWindow() {
    delete ui;
}
