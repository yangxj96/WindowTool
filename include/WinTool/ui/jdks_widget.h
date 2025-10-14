#ifndef WINTOOL_JDKS_WIDGET_H
#define WINTOOL_JDKS_WIDGET_H

#include <QWidget>


class ConfigManager;
QT_BEGIN_NAMESPACE

namespace Ui {
    class JdksWidget;
}

QT_END_NAMESPACE

class JdksWidget : public QWidget {
    Q_OBJECT

public:
    explicit JdksWidget(QWidget* parent = nullptr);

    ~JdksWidget() override;

protected:
    void showEvent(QShowEvent* event) override;

private:
    Ui::JdksWidget* ui;

    bool m_loaded = false;

    ConfigManager* m_config;

    // JDK前缀
    QString jdks_prefix;

    // 设置table的数据
    void setTableData() const;

private slots:

    void on_btn_jdk_prefix_pick_clicked();
};


#endif //WINTOOL_JDKS_WIDGET_H