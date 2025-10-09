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

public:
    explicit ServiceWidget(QWidget* parent = nullptr);

    ~ServiceWidget() override;

private:
    Ui::ServiceWidget* ui;

    ConfigManager* m_config;

    QList<ServiceInfo> m_services;

    // 初始化UI内容
    void initUi() const;

    // 读取配置获取数据
    void loadServices();

    // 设置table的数据
    void setTableData() const;

    // 轮询查询状态
    void pollServiceStatus(const ServiceInfo&service, DWORD targetState,
        const QString&successMsg, const QString&failureMsg);

private slots:
    void on_btn_auto_start_clicked();

    void on_btn_auto_stop_clicked();

    // table相关槽

    void on_tb_services_customContextMenuRequested(const QPoint&pos);
};


#endif //WINTOOL_SERVICE_WIDGET_H
