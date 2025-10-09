#include "WinTool/ui/service_widget.h"
#include "ui_service_widget.h"
#include <QMessageBox>
#include <QMenu>

#include "ServiceInfo.h"

ServiceWidget::ServiceWidget(QWidget* parent) : QWidget(parent), ui(new Ui::ServiceWidget) {
    ui->setupUi(this);
    // 写入配置
    QList<ServiceInfo> service_infos = {
        {"张三", 12},
        {"李四", 30},
        {"王五", 28}
    };

    this->initUi();
    // 填充数据
    this->setTableData();
}

ServiceWidget::~ServiceWidget() {
    delete ui;
}

void ServiceWidget::initUi() const {
    // 设置表头
    ui->tb_services->setHorizontalHeaderLabels({"显示名称", "服务名称", "统一启动", "状态"});
}

void ServiceWidget::setTableData() const {
    // 先根据数据设置行数
    ui->tb_services->setRowCount(3);
    // 添加数据
    ui->tb_services->setItem(0, 0, new QTableWidgetItem("MySQL"));
    ui->tb_services->setItem(0, 1, new QTableWidgetItem("MySQL"));
    ui->tb_services->setItem(0, 2, new QTableWidgetItem("🚀是"));
    ui->tb_services->setItem(0, 3, new QTableWidgetItem("运行中"));

    ui->tb_services->setItem(1, 0, new QTableWidgetItem("PGSQL"));
    ui->tb_services->setItem(1, 1, new QTableWidgetItem("PostgreSQL"));
    ui->tb_services->setItem(1, 2, new QTableWidgetItem("❤️是"));
    ui->tb_services->setItem(1, 3, new QTableWidgetItem("运行中"));

    ui->tb_services->setItem(2, 0, new QTableWidgetItem("达梦(RICS)"));
    ui->tb_services->setItem(2, 1, new QTableWidgetItem("DmServiceDMSERVER"));
    ui->tb_services->setItem(2, 2, new QTableWidgetItem("😊是"));
    ui->tb_services->setItem(2, 3, new QTableWidgetItem("运行中"));

    // 自动调整列宽
    ui->tb_services->resizeColumnsToContents();
    ui->tb_services->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ServiceWidget::on_btn_auto_start_clicked() {
    QMessageBox::information(this, "提示", "一键启动被点击");
}

void ServiceWidget::on_btn_auto_stop_clicked() {
    QMessageBox::information(this, "提示", "一键停止被点击");
}

void ServiceWidget::on_tb_services_customContextMenuRequested(const QPoint&pos) const {
    // 获取点击位置的 item
    const QTableWidgetItem* item = ui->tb_services->itemAt(pos);
    if (!item) return;

    int row = item->row();

    QMenu menu;
    menu.addAction("启动服务", [this, row]() {
        QMessageBox::information(nullptr, "提示", "启动服务");
    });

    // ✅ 推荐：菜单出现在鼠标右下方，不遮挡
    menu.exec(ui->tb_services->mapToGlobal(pos) + QPoint(12, 12));
}
