#include "WinTool/ui/main_window.h"
#include "ui_main_window.h"

#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->widget_service = new ServiceWidget(this);
    this->widget_jdks = new JdksWidget(this);
    this->widget_misc = new Misc(this);

    ui->tw->addTab(widget_service, tr("服务管理"));
    ui->tw->addTab(widget_jdks, tr("JDK管理"));
    ui->tw->addTab(widget_misc, tr("杂项"));

    // 👉 连接进度信号
    connect(widget_service, &ServiceWidget::progressUpdated, this, &MainWindow::onServiceProgressUpdated);
    connect(widget_service, &ServiceWidget::processingStarted, this, &MainWindow::onServiceProcessingStarted);
    connect(widget_service, &ServiceWidget::processingFinished, this, &MainWindow::onServiceProcessingFinished);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onServiceProgressUpdated(const int current, const int total, const QString&serviceName) const {
    const QString text = QString("服务处理中: %1/%2 - %3")
            .arg(current)
            .arg(total)
            .arg(serviceName);
    statusBar()->showMessage(text);
}

void MainWindow::onServiceProcessingStarted() const {
    // 可以加个图标或样式
    statusBar()->setStyleSheet("color: blue;");
}

void MainWindow::onServiceProcessingFinished() const {
    statusBar()->clearMessage();
    statusBar()->setStyleSheet(""); // 恢复样式
}
