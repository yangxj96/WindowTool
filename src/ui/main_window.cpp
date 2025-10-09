#include "WinTool/ui/main_window.h"
#include "ui_main_window.h"

#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // // 创建一个用于显示消息的 QLabel
    // m_statusRightLabel = new QLabel("就绪");
    // m_statusRightLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 文字靠右
    // m_statusRightLabel->setMinimumWidth(200);                             // 设置最小宽度，避免跳动
    // m_statusRightLabel->setStyleSheet("QLabel { color: #333; padding: 2px; }");

    // 👉 使用 addPermanentWidget 将其添加到状态栏右侧
    // statusBar()->addPermanentWidget(m_statusRightLabel);

    this->widget_service = new ServiceWidget(this);
    this->widget_jdks = new JdksWidget(this);

    ui->tw->addTab(widget_service, tr("服务管理"));
    ui->tw->addTab(widget_jdks, tr("JDK管理"));

    // 👉 连接进度信号
    connect(widget_service, &ServiceWidget::progressUpdated   ,this, &MainWindow::onServiceProgressUpdated);
    connect(widget_service, &ServiceWidget::processingStarted ,this, &MainWindow::onServiceProcessingStarted);
    connect(widget_service, &ServiceWidget::processingFinished,this, &MainWindow::onServiceProcessingFinished);
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
