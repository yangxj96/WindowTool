#ifndef WINTOOL_SERVICE_WIDGET_H
#define WINTOOL_SERVICE_WIDGET_H

#include <QWidget>

#include "WinTool/helper/config_manager.h"
#include "WinTool/helper/service_helper.h"


struct ServiceInfo;


QT_BEGIN_NAMESPACE

namespace Ui {
    class ServiceWidget;
}

QT_END_NAMESPACE

class ServiceWidget : public QWidget {
    Q_OBJECT

signals:

    // 发出当前处理进度：当前第几个，总共几个，当前服务名
    void progressUpdated(int current, int total, const QString& serviceName);

    // 任务开始信号
    void processingStarted();

    //任务结束信号
    void processingFinished();

public:
    explicit ServiceWidget(QWidget* parent = nullptr);

    ~ServiceWidget() override;

private:
    Ui::ServiceWidget* ui;

    ConfigManager* m_config;

    QList<ServiceInfo> m_services;


    //    队列
    struct ServiceTask {
        ServiceInfo service;
        DWORD targetState; // SERVICE_RUNNING 或 SERVICE_STOPPED
        QString actionName; // "启动" 或 "停止"
    };
    // 任务队列
    QList<ServiceTask> m_queue_services;
    // 当前队列总数
    int m_queue_total = 0;
    // 当前队列第几个
    int m_queue_current_idx = 0;
    // 当前队列处理的服务
    ServiceInfo m_queue_current_service;
    // 当前队列的目标状态
    DWORD m_queue_current_target_state = 0;
    // 当前操作名称,启动/停止
    QString m_queue_current_action_name;
    // 是否正在处理队列
    bool m_queue_is_processing = false;
    // 共享的轮询定时器
    QTimer* m_queue_timer = nullptr;
    //    队列


    // 初始化UI内容
    void initUi() const;

    // 读取配置获取数据
    void loadServices();

    // 设置table的数据
    void setTableData() const;

    // 轮询查询状态
    void pollServiceStatus(const ServiceInfo&service, DWORD targetState,
        const QString&successMsg, const QString&failureMsg);

    void startServiceQueue();

    void processNextInQueue();

private slots:

    void onPollTimerTick();

    void on_btn_auto_start_clicked();

    void on_btn_auto_stop_clicked();

    // table相关槽

    void on_tb_services_customContextMenuRequested(const QPoint&pos);
};


#endif //WINTOOL_SERVICE_WIDGET_H
