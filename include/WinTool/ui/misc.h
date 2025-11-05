#ifndef WINTOOL_MISC_H
#define WINTOOL_MISC_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class misc;
}

QT_END_NAMESPACE

class Misc : public QWidget {
    Q_OBJECT

public:
    explicit Misc(QWidget* parent = nullptr);

    ~Misc() override;

private:
    Ui::misc* ui;

private slots:
    void on_btn_navicat_cleanup_clicked();
};


#endif //WINTOOL_MISC_H
