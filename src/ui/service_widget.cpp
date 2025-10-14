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
}

ServiceWidget::~ServiceWidget() {
    delete ui;
}

void ServiceWidget::showEvent(QShowEvent* event) {
    if (!m_loaded) {
        // 创建共享轮询定时器
        m_queue_timer = new QTimer(this);
        connect(m_queue_timer, &QTimer::timeout, this, &ServiceWidget::onPollTimerTick);

        // 读取配置后填充数据
        this->loadServices();

        m_loaded = true;
    }
    QWidget::showEvent(event);
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

void ServiceWidget::startServiceQueue() {
    if (m_queue_is_processing || m_queue_services.isEmpty()) {
        return;
    }

    m_queue_is_processing = true;

    m_queue_total = m_queue_services.size();
    m_queue_current_idx = 0;

    emit processingStarted();
    emit progressUpdated(m_queue_current_idx, m_queue_total, "准备中...");

    // 禁用 UI
    ui->tb_services->setEnabled(false);
    ui->btn_auto_start->setEnabled(false);
    ui->btn_auto_stop->setEnabled(false);

    processNextInQueue();
}

void ServiceWidget::processNextInQueue() {
    // 为空则说明队列没了,全都执行完成了
    if (m_queue_services.isEmpty()) {
        // 发送槽信号
        emit processingFinished();
        // 最终刷新
        loadServices();
        m_queue_is_processing = false;

        // 恢复 UI
        ui->tb_services->setEnabled(true);
        ui->btn_auto_start->setEnabled(true);
        ui->btn_auto_stop->setEnabled(true);
        return;
    }

    const auto [service, targetState, actionName] = m_queue_services.takeFirst();

    m_queue_current_idx++;
    m_queue_current_service = service;
    m_queue_current_target_state = targetState;
    m_queue_current_action_name = actionName;

    emit progressUpdated(m_queue_current_idx, m_queue_total, m_queue_current_service.display_name);

    qDebug() << "开始" << m_queue_current_action_name << "服务:" << service.display_name;

    // 发送控制命令
    bool started = false;
    if (targetState == SERVICE_RUNNING) {
        started = ServiceHelper::start(service.service_name);
    }
    else {
        started = ServiceHelper::stop(service.service_name);
    }

    if (!started) {
        QMessageBox::warning(this, "失败", service.display_name + " " + m_queue_current_action_name + "失败（发送命令失败）");
        processNextInQueue(); // 继续下一个
        return;
    }

    // 开始轮询该服务的状态
    m_queue_timer->start(1000); // 每秒查询一次
}

void ServiceWidget::onPollTimerTick() {
    SERVICE_STATUS status;
    if (!ServiceHelper::query(m_queue_current_service.service_name, status)) {
        QMessageBox::warning(this, "查询失败", "无法获取服务状态: " + m_queue_current_service.display_name);
        m_queue_timer->stop();
        processNextInQueue(); // 进入下一个任务
        return;
    }

    if (status.dwCurrentState == m_queue_current_target_state) {
        // ✅ 成功
        qDebug() << m_queue_current_service.display_name << "已进入目标状态";
        m_queue_timer->stop();
        processNextInQueue(); // 成功，继续下一个
        return;
    }

    if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_STOPPED) {
        // ❌ 已进入终态，但不是目标状态（如启动失败自动停止）
        QMessageBox::warning(this, "状态异常", m_queue_current_service.display_name + " 未达到预期状态");
        m_queue_timer->stop();
        processNextInQueue();
    }
}

void ServiceWidget::on_btn_auto_start_clicked() {
    m_queue_services.clear();
    m_queue_current_idx = 0;
    for (const auto&service: m_services) {
        if (!service.unify) continue;
        if (SERVICE_STATUS status; ServiceHelper::query(service.service_name, status)) {
            if (status.dwCurrentState != SERVICE_RUNNING) {
                m_queue_services.append({service,SERVICE_RUNNING, "启动"});
            }
        }
    }
    if (m_queue_services.isEmpty()) {
        QMessageBox::information(this, "提示", "所有统一服务已在运行");
        return;
    }
    startServiceQueue();
}

void ServiceWidget::on_btn_auto_stop_clicked() {
    m_queue_services.clear();
    for (const auto&service: m_services) {
        if (SERVICE_STATUS status; ServiceHelper::query(service.service_name, status)) {
            if (status.dwCurrentState != SERVICE_STOPPED) {
                m_queue_services.append({service,SERVICE_STOPPED, "停止"});
            }
        }
    }
    if (m_queue_services.isEmpty()) {
        QMessageBox::information(this, "提示", "所有统一服务已在停止");
        return;
    }
    startServiceQueue();
}

void ServiceWidget::on_btn_service_add_clicked() {
    QMessageBox::information(nullptr, "提示", "服务新增被单击");
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

        m_queue_services.clear();
        m_queue_services.append({service,SERVICE_RUNNING, "启动"});
        startServiceQueue();
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

        m_queue_services.clear();
        m_queue_services.append({service,SERVICE_STOPPED, "停止"});
        startServiceQueue();
    });

    // ✅ 推荐：菜单出现在鼠标右下方，不遮挡
    menu.exec(ui->tb_services->mapToGlobal(pos) + QPoint(12, 12));
}
