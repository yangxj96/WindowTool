#ifndef WINTOOL_JDKS_WIDGET_H
#define WINTOOL_JDKS_WIDGET_H

#include <QWidget>


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

private:
    Ui::JdksWidget* ui;
};


#endif //WINTOOL_JDKS_WIDGET_H