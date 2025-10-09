#include "../../include/WinTool/ui/JdksWidget.h"
#include "ui_JdksWidget.h"

JdksWidget::JdksWidget(QWidget* parent) : QWidget(parent), ui(new Ui::JdksWidget) {
    ui->setupUi(this);
}

JdksWidget::~JdksWidget() {
    delete ui;
}