#include "WinTool/ui/service_widget.h"


#include "ui_service_widget.h"
#include <QMessageBox>
#include <QMenu>
#include <QTimer>
#include <QDebug>

#include "WinTool/helper/service_helper.h"
#include "WinTool/helper/config_manager.h"

ServiceWidget::ServiceWidget(QWidget* parent) : QWidget(parent), ui(new Ui::ServiceWidget), m_config(&ConfigManager::instance()) {
    ui->setupUi(this);

    // 初始化UI
    this->initUi();
    // 填充数据
    // this->setTableData();

    // 读取配置后填充数据
    this->loadServices();
}

ServiceWidget::~ServiceWidget() {
    delete ui;
}

void ServiceWidget::initUi() const {
    // 设置表头
    ui->tb_services->setHorizontalHeaderLabels({"显示名称", "服务名称", "统一启动", "状态"});
}

void ServiceWidget::loadServices() {
    auto jsonArray = m_config->get<QJsonArray>("services");
    m_services.clear(); // 防止重复加载

    for (const auto&value: jsonArray) {
        auto serviceInfo = ServiceInfo::fromJsonObject(value.toObject());
        m_services.append(serviceInfo);
    }

    this->setTableData();
}

// void ServiceWidget::readData() const {
//     auto&config = ConfigManager::instance();
//
//     // @formatter:off
//     QJsonArray services;
//     services.append(ServiceInfo("RICS"   ,"DmServiceDMSERVER",true).toJsonObject());
//     services.append(ServiceInfo("SPECTRA","DmServiceDMSERVER",true).toJsonObject());
//     services.append(ServiceInfo("DMAP"   ,"DmAPService"      ,true).toJsonObject());
//     services.append(ServiceInfo("Redis"  ,"Redis"            ,true).toJsonObject());
//     services.append(ServiceInfo("MySQL"  ,"MySQL"            ,false).toJsonObject());
//     services.append(ServiceInfo("PGSQL"  ,"PostgreSQL"       ,false).toJsonObject());
//     services.append(ServiceInfo("INODE"  ,"INODE_SVR_SERVICE",false).toJsonObject());
//     config.set("services",services);
//     // @formatter:on
//
//     for (auto services = config.get<QJsonArray>("services"); const auto&service: services) {
//         auto [display_name, service_name, unify] = ServiceInfo::fromJsonObject(service.toObject());
//         qDebug() << "显示名称:" << display_name << ";服务名称:" << service_name << ";统一启动" << unify;
//     }
// }

void ServiceWidget::setTableData() const {
    // 先根据数据设置行数
    ui->tb_services->setRowCount(m_services.size());

    for (int row = 0; row < m_services.size(); ++row) {
        const auto&[display_name, service_name, unify] = m_services[row];

        SERVICE_STATUS status;
        const bool ok = ServiceHelper::query(service_name, status);
        QString statusText = ok ? ServiceHelper::statusToString(status.dwCurrentState) : "❌ 查询失败";

        ui->tb_services->setItem(row, 0, new QTableWidgetItem(display_name));
        ui->tb_services->setItem(row, 1, new QTableWidgetItem(service_name));
        ui->tb_services->setItem(row, 2, new QTableWidgetItem(unify ? "是" : "否"));
        ui->tb_services->setItem(row, 3, new QTableWidgetItem(statusText));
    }

    // 自动调整列宽
    ui->tb_services->resizeColumnsToContents();
    ui->tb_services->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ServiceWidget::pollServiceStatus(const ServiceInfo&service, DWORD targetState,
    const QString&successMsg, const QString&failureMsg) {

    ui->tb_services->setEnabled(false);
    ui->btn_auto_start->setEnabled(false);
    ui->btn_auto_stop->setEnabled(false);

    // 使用单次定时器实现轮询
    QTimer::singleShot(1000, [this, service, targetState, successMsg, failureMsg]() mutable {
        if (SERVICE_STATUS status; ServiceHelper::query(service.service_name, status)) {
            if (status.dwCurrentState == targetState) {
                // ✅ 目标状态达成
                // QMessageBox::information(nullptr, "成功", service.display_name + " " + successMsg);
                loadServices(); // 最终刷新 UI
                return;
            }

            if (status.dwCurrentState == SERVICE_STOPPED || status.dwCurrentState == SERVICE_RUNNING) {
                // ❌ 进入了非目标的终态（比如启动失败自动停止）
                QMessageBox::warning(nullptr, "状态异常", service.display_name + " 未达到预期状态");
                loadServices();
                return;
            }

            // ❓ 仍在过渡状态（如 START_PENDING），继续轮询
            pollServiceStatus(service, targetState, successMsg, failureMsg);
        }
        else {
            // 查询失败，可能服务已不存在或权限问题
            QMessageBox::warning(nullptr, "查询失败", "无法获取服务状态，停止轮询");
            loadServices();
        }
        ui->tb_services->setEnabled(true);
        ui->btn_auto_start->setEnabled(true);
        ui->btn_auto_stop->setEnabled(true);
    });
}

void ServiceWidget::on_btn_auto_start_clicked() {
    QMessageBox::information(this, "提示", "一键启动被点击");
}

void ServiceWidget::on_btn_auto_stop_clicked() {
    QMessageBox::information(this, "提示", "一键停止被点击");
}

void ServiceWidget::on_tb_services_customContextMenuRequested(const QPoint&pos) {
    // 获取点击位置的 item
    const QTableWidgetItem* item = ui->tb_services->itemAt(pos);
    if (!item) return;

    int row = item->row();

    QMenu menu;
    menu.addAction("启动服务", [this, row] {
        const auto service = m_services[row];
        qDebug() << "尝试启动服务:" << service.display_name;

        SERVICE_STATUS status;
        if (!ServiceHelper::query(service.service_name, status)) {
            QMessageBox::warning(nullptr, "错误", "无法查询服务状态");
            return;
        }

        if (status.dwCurrentState == SERVICE_RUNNING) {
            QMessageBox::information(nullptr, "提示", "服务已经是运行状态");
            return;
        }

        if (status.dwCurrentState == SERVICE_START_PENDING) {
            QMessageBox::information(nullptr, "提示", "服务正在启动中，请稍候...");
            return;
        }

        if (ServiceHelper::start(service.service_name)) {
            QMessageBox::information(nullptr, "提示", "已发送启动请求");

            // 开始轮询状态
            pollServiceStatus(service, SERVICE_RUNNING, "启动成功", "启动失败");
        }
        else {
            QMessageBox::critical(nullptr, "错误", "启动服务失败，请检查权限或服务配置");
        }
    });

    menu.addAction("停止服务", [this,row] {
        const auto service = m_services[row];
        qDebug() << "尝试停止服务:" << service.display_name;

        SERVICE_STATUS status;
        if (!ServiceHelper::query(service.service_name, status)) {
            QMessageBox::warning(nullptr, "错误", "无法查询服务状态");
            return;
        }

        if (status.dwCurrentState == SERVICE_STOPPED) {
            QMessageBox::information(nullptr, "提示", "服务已经是停止状态");
            return;
        }

        if (status.dwCurrentState == SERVICE_STOP_PENDING) {
            QMessageBox::information(nullptr, "提示", "服务正在停止中，请稍候...");
            return;
        }

        if (ServiceHelper::stop(service.service_name)) {
            QMessageBox::information(nullptr, "提示", "已发送停止请求");

            // 开始轮询状态
            pollServiceStatus(service, SERVICE_STOPPED, "停止成功", "停止失败");
        }
        else {
            QMessageBox::critical(nullptr, "错误", "停止服务失败，请检查权限或服务是否可停止");
        }
    });

    // ✅ 推荐：菜单出现在鼠标右下方，不遮挡
    menu.exec(ui->tb_services->mapToGlobal(pos) + QPoint(12, 12));
}
