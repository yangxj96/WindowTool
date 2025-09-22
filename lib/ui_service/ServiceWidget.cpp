#include "ServiceWidget.h"
#include "ui_ServiceWidget.h"
#include <QMessageBox>

ServiceWidget::ServiceWidget(QWidget* parent) : QWidget(parent), ui(new Ui::ServiceWidget) {
    ui->setupUi(this);
    this->initUi();
}

ServiceWidget::~ServiceWidget() {
    delete ui;
}

void ServiceWidget::initUi() const {
    // 1. 设置行列数
    ui->tb_services->setRowCount(2);
    ui->tb_services->setColumnCount(2);

    // 2. 设置表头
    ui->tb_services->setHorizontalHeaderLabels({"姓名", "年龄"});

    // 3. 添加数据
    ui->tb_services->setItem(0, 0, new QTableWidgetItem("张三"));
    ui->tb_services->setItem(0, 1, new QTableWidgetItem("25"));

    ui->tb_services->setItem(1, 0, new QTableWidgetItem("李四"));
    ui->tb_services->setItem(1, 1, new QTableWidgetItem("30"));

    // 4. 设置整行选中
    ui->tb_services->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 5. 双击可编辑
    ui->tb_services->setEditTriggers(QAbstractItemView::DoubleClicked);

    // 6. 自动调整列宽
    ui->tb_services->resizeColumnsToContents();
}

void ServiceWidget::on_btn_auto_start_clicked() {
    QMessageBox::information(this, "提示", "一键启动被点击");
}

void ServiceWidget::on_btn_auto_stop_clicked() {
    QMessageBox::information(this, "提示", "一键停止被点击");
}

void ServiceWidget::on_btn_checked_start_clicked() {
    QMessageBox::information(this, "提示", "选中启动被点击");
}

void ServiceWidget::on_btn_checked_stop_clicked() {
    QMessageBox::information(this, "提示", "选中停止被点击");
}
