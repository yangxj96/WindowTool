#ifndef WINTOOL_SERVICE_WIDGET_H
#define WINTOOL_SERVICE_WIDGET_H

#include <QWidget>


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

    // 初始化UI内容
    void initUi() const;

    // 设置table的数据
    void setTableData() const;

private slots:
    void on_btn_auto_start_clicked();

    void on_btn_auto_stop_clicked();

    // table相关槽

    void on_tb_services_customContextMenuRequested(const QPoint &pos) const;
};


#endif //WINTOOL_SERVICE_WIDGET_H
